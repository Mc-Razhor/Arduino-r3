//Creado por Antonio Zamorano, Universidad Autonoma de Nayarit
//Este codigo es de dominio publico, puede ser usado para cualquier proposito.

#include <SPI.h>  //Importamos librería comunicación SPI
#include <Ethernet.h>  //Importamos librería Ethernet
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Ponemos la dirección MAC de la Ethernet Shield que está con una etiqueta debajo la placa
IPAddress ip(192,168,1,50); //Asingamos la IP al Arduino
IPAddress gateway(192,168,1, 1); // Puerta de enlace o Router
IPAddress subnet(255, 255, 255, 0); //Mascara de Sub Red (Subnet Mask)
EthernetServer server(80); //Creamos un servidor Web con el puerto 80 que es el puerto HTTP por defecto

int riego=2; //Pin de Arduino asignado a la riego
int sistema=3; //Pin de Arduino asignado al sistema
String estado1="OFF"; //Estado de la riego inicialmente en "OFF"
String estado2="OFF"; //Estado del sistema inicialmente en "OFF"
String estado3="HUMEDO";
int estadoriego=1;
 
void setup()
{
  Serial.begin(9600);
 
  // Inicializamos la comunicación Ethernet y el servidor
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
 
  pinMode(riego,OUTPUT);
  pinMode(sistema,OUTPUT);
}
 
void loop()
{
  EthernetClient client = server.available(); //Creamos un cliente Web
  //Cuando detecte un cliente a través de una petición HTTP
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
    String cadena=""; //Creamos una cadena de caracteres vacía
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();//Leemos la petición HTTP carácter por carácter
        Serial.write(c);//Visualizamos la petición HTTP por el Monitor Serial
        cadena.concat(c);//Unimos el String 'cadena' con la petición HTTP (c). De esta manera convertimos la petición HTTP a un String


            int puertoAnalogo = 0;
            int lecturaSensor = analogRead(puertoAnalogo); 
            
            if(lecturaSensor<490){ //MONITOREO DE HUMEDAD
            estado3="HUMEDO";

  
            }
            if(lecturaSensor>490){ //MONITOREO DE HUMEDAD
            estado3="SECO";
            } 
          
 
         //Ya que hemos convertido la petición HTTP a una cadena de caracteres, ahora podremos buscar partes del texto.
         
          int posicion1=cadena.indexOf("RIEGO="); //Guardamos la posición de la instancia "riego=" a la variable 'posicion1'
 
          if(cadena.substring(posicion1)=="RIEGO=ON" || estado3=="SECO" && estadoriego==1)//Si a la posición 'posicion1' hay "riego=ON"
          {
            estado1="ON";
            digitalWrite(riego,LOW);
            
          }
          if(cadena.substring(posicion1)=="RIEGO=OFF" || estado3=="HUMEDO" || estadoriego==0)//Si a la posición 'posicion1' hay "riego=OFF"
          {
            estado1="OFF";
            digitalWrite(riego,HIGH);

          }
 
       
         int posicion2=cadena.indexOf("SISTEMA="); //Guardamos la posición de la instancia "SISTEMA=" a la variable 'posicion2'
 
          if(cadena.substring(posicion2)=="SISTEMA=ON")//Si a la posición 'posicion2' hay "sistema=ON"
          {
            digitalWrite(sistema,HIGH);
            estado2="ON";
            estadoriego=1;
          }
          if(cadena.substring(posicion2)=="SISTEMA=OFF")//Si a la posición 'posicion2' hay "SISTEMA=OFF"
          {
            digitalWrite(sistema,LOW);
            estado2="OFF";
            estadoriego=0;
          }
          

  
        //Cuando reciba una línea en blanco, quiere decir que la petición HTTP ha acabado y el servidor Web está listo para enviar una respuesta
        if (c == '\n' && currentLineIsBlank) {

            // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Refresh: 3");  // refresca la pagina automaticamente cada 3 segundos
            client.println();
 
            //Página web en formato HTML
            client.println("<html>");
            client.println("<head>");
            client.println("</head>");
            client.println("<body>");
            client.println("<body style='background:#E1F5A9'>");
            client.println("<h1>Sistema de Riego Automatizado</h1><h4>Universidad Autonoma de Nayarit</h4><h2>Control de sistema de riego</h2>");
            //Creamos los botones. Para enviar parametros a través de HTML se utiliza el metodo URL encode. Los parámetros se envian a través del símbolo '?'
            client.println("<div>");
            client.println("</b><br />");
            client.println("<button onClick=location.href='./?SISTEMA=ON\' style='margin:auto;background-color: #84B1FF;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("ON");
            client.println("</button>");
            client.println("<button onClick=location.href='./?SISTEMA=OFF\' style='margin:auto;background-color: #84B1FF;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("OFF");
            client.println("</button>");
            client.println("<br /><br />");
            client.println("<b>SISTEMA = ");
            client.print(estado2);
            client.println("</b><br />");
            client.println("<b>ESTADO DE BOMBA DE AGUA= ");
            client.print(estado1);
            client.println("</b><br />");
            client.println("</b></body>");
            client.println("<hr />");
            client.println("<H2>Monitoreo de Humedad</H2>");
            client.println("<br />");  
            client.println("Estado del Suelo: ");
            client.println(estado3);
            client.println("<br />");       
            client.println("<br />"); 
            client.println("UAN Carreras de las TIC"); 
            client.println("</html>");
            break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    //Dar tiempo al navegador para recibir los datos
    delay(3);
    client.stop();// Cierra la conexión
  }
}
