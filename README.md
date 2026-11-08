Practice 2 — Languages and Paradigms of Computation 

Integrantes:
Luisa Maria Acevedo Agudelo

Sara Valenthyna Echeverry Panesso

Descripción del proyecto

Programa desarrollado en C++17 que simula el cobro de tarifas del sistema integrado de transporte del Valle de Aburrá.

Implementa diferentes medios de transporte y perfiles de pasajeros mediante herencia y polimorfismo. También aplica una ventana de integración de 90 minutos, permite hasta dos transbordos gratuitos y genera reportes utilizando algoritmos de la STL.

Compilación y ejecución

Para compilar el archivo main.cpp, ejecutar:

g++ -std=c++17 -Wall -Wextra -o practice main.cpp

Para ejecutar el programa:

./practice

En Windows (PowerShell), utilizar .\practice.exe si el ejecutable se generó con ese nombre.

Preguntas de sustentación:
1. ¿Qué ocurre si el destructor de TransportMode no es virtual?

En nuestro programa utilizamos una clase padre llamada TransportMode, de la cual heredan los diferentes medios de transporte, como MetroLine, CableLine y TouristCable.
Como estos objetos se almacenan mediante unique_ptr<TransportMode>, necesitamos que el destructor de la clase padre sea virtual.
Esto permite que, cuando se elimine un transporte, se ejecute correctamente el destructor de la clase hija correspondiente y después el de la clase padre.
Si no utilizáramos virtual, al destruir un objeto de una clase hija mediante un puntero de tipo TransportMode, se produciría un comportamiento indefinido.
Por eso declaramos el destructor virtual: para garantizar la destrucción correcta de los objetos y la administración adecuada de sus recursos.

2. ¿Por qué describe() no es virtual, aunque utiliza funciones virtuales?

El método describe() se encarga de construir la descripción de cada medio de transporte, mostrando información como su nombre, número de estaciones, tipo de servicio y tarifa.
No necesitamos declararlo virtual porque todos los transportes utilizan el mismo procedimiento para mostrar esa información.
Sin embargo, dentro de describe() llamamos a métodos virtuales como baseFare() y serviceType(), que tienen implementaciones diferentes según el transporte.
Por ejemplo, cuando utilizamos describe() con un objeto MetroLine, muestra la información del metro y su tarifa de $3.820. Si lo utilizamos con TouristCable, muestra la información del cable turístico y su tarifa con el recargo correspondiente.
De esta manera, reutilizamos el mismo método sin repetir código en cada clase hija, aprovechando el polimorfismo.
