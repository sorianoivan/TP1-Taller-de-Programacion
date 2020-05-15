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

### <u>Reentrega</u>

A continuación se listan los cambios realizados para la reentrega:

##### Issue 1: Send puede retornar 0:

Se agrega un chequeo para que el programa no quede en un bucle infinito en caso de que *send()* retorne 0

##### Issue 2: Funciones try:

Se quito el *try* de las funciones que lo tenían. Ejemplo: *try_send* -> *socket_send*

##### Issue 3: Typo:

recieve -> receive

##### Issue 4: Documentacion:

Se agrego documentación a aquellas funciones cuya logica no era clara

##### Issue 5: strtok_r():

Se implementaron funciones utilizando strstr() que cumplen la función que antes era de strtok_r(). Estas funciones son *_set_header_param()*, *_set_body_parameters()*, *_get_param()*

##### Issue 6: Refactorización:

El paso de host a little endian y viceversa se paso a las funciones *mp_host_to_little()* y *mp_little_to_host()*

##### Issue 7: Leak:

El leak ocurría si los argumentos recibidos eran inválidos, así que se movió el constructor del TDA Client para que se llame solo si los argumentos son válidos

##### Issue 8: Usar sizeof():

Se agrego sizeof() al llamado de fgets. Ahora es:

`if (fgets(buff, (BUFF_SIZE + 1) * sizeof(char), file) != NULL)` 

##### Issue 9: Encapsulamiento TDA Socket:

Se agrego constructor y destructor del TDA Socket para que no se rompa el encapsulamiento

##### Issue 10: Heap innecesario:

Se pasa a usar un array fijo en lugar de uno dinámico

##### Issue 11: Falta Documentacion:

Idem Issue 4