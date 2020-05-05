Nombre: Ivan Soriano

Padrón: 102342

URL: https://github.com/sorianoivan/tp1.git



# Trabajo Práctico 1



### <u>Envío del mensaje</u>

Luego de lograr establecer una conexión con el server, el cliente se encarga de leer linea por linea el archivo si se recibió uno por parámetro, o la entrada estándar en caso contrario. Luego de leer una linea, se la envía al TDA *common_message_processor* , el cual arma el mensaje que debe ser enviado al server utilizando el protocolo DBus.

 Para armar el mensaje, *common_message_processor*  guarda cada parte de la linea leída (ruta, destino, interfaz, método y parámetros) en distintas variables. Luego, si es que el mensaje tiene cuerpo, procede a armarlo, para lo cual toma la variable que guarda todos los parámetros y de ahí lee cada parámetro secuencialmente y lo agrega, junto a su largo, a la variable que almacena el cuerpo. Cuando finaliza de armar el cuerpo pasa al armado de la cabecera, el cual sigue la misma lógica que el armado del cuerpo, con la diferencia de que antes de agregar las distintas partes de se debe agregar la información de todo el mensaje, como su id, la longitud tanto del cuerpo como la cabecera y el endianess que se utilizara, entre otras cosas. Por ultimo, se concatena el cuerpo al final de la cabecera y se devuelve el mensaje completo. 

Una vez que el cliente tiene el mensaje armado, se lo envía al server mediante la conexión previamente establecida y luego espera la respuesta del servidor. La respuesta es enviada a *common_printer* que la imprime por pantalla. Luego continua leyendo otra linea o, en caso de que ya no se encuentren mas lineas para procesar, se comienza la secuencia de finalizacion del programa que consiste en cerrar el archivo del cual se leyo y cerrar el socket. A continuación se adjunta un diagrama que muestra esta secuencia:

![Envio_Mensaje_Diagrama_Secuencia](/img/Envio_Mensaje_Diagrama_Secuencia.png)

### <u>Recepción Mensaje</u>

Del lado del server, luego de establecer la conexión, este recibe, mediante su *peer_skt* el mensaje que le envía el cliente. Para recibir el mensaje lo primero que hace es leer los primeros 16 bytes, ya que estos poseen información sobre todo el mensaje que me permitirá alocar la memoria. Luego de alocar dicha memoria recibe el mensaje completo, que es enviado a *common_printer* que lo separa en sus distintas partes para mostrarlo tal como se pide en el enunciado. Luego, si el cliente envió otro mensaje este sera procesado de igual manera, en caso contrario, se termina la ejecución del server cerrando los sockets *bind* y *peer* Como con el envió del mensaje, a continuación se muestra un diagrama que muestra esta secuencia

![Recepcion_Mensaje_Diagrama_Secuencia](/img/Recepcion_Mensaje_Diagrama_Secuencia.png)

### Aclaraciones

Luego de lograr que mi trabajo pase exitosamente las pruebas de SERCOM comencé a modularizarlo, ya que estaba muy desprolijo y no cumplía con algunas reglas de la cátedra. Termine de modularizar todo la noche del lunes previa a la entrega, pero al hacer la entrega en SERCOM esta falló, indicando en Observaciones "Se esperaba terminar con un código de retorno 0 pero se obtuvo 7", lo cual me pareció raro, ya que mi programa devuelve 0 si todo funciono bien o -1 si hay un error. Por esto vi todos los archivos de salida que provee el SERCOM pero el único que indicaba un error era __ stdout __ diciendo "Client/Netcat 0 exit code: 255. ERROR: Unexpected result in Client/Netcat 0 execution.". Al ver esto ejecute mi programa con los 3 archivos de prueba y en todos los casos funciono bien y devolvió 0. Luego, pensando que podía ser un problema de como lo estaba ejecutando, descargue los archivos "free_port.sh" y "run.sh" del SERCOM y corri el comando "chmod +x run.sh free_port.sh;./run.sh $(./free_port.sh 10001) no-valgrind" y al contrario del SERCOM, la prueba paso. Dejo una captura:

![Prueba SERCOM](/img/Prueba_SERCOM_OK.png)

Al preguntar por la lista, un ayudante me dijo que corriéndolo detecto un leak, pero cuando yo lo corrí con los mismos flag, valgrind no detecto nada:

![Valgrind](/img/Valgrind_No_Leaks.png)

Dado que mi pc no detectaba el problema no tuve la posibilidad de arreglarlo para la entrega. Por esto, la ultima entrega que pasa las pruebas es una versión muy temprana que no es aceptable como entrega final. Por eso agradecería que para corregir se tome la ultima entrega al SERCOM a pesar de que haya fallado.
