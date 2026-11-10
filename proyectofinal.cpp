#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// tamanos de los arreglos (constantes porque el tamano de un arreglo no puede cambiar)
const int PRODUCTOS=6; // cantidad de productos del menu
const int MESAS=6; // cantidad de mesas del restaurante
const int MAXORDENES=200; // maximo de ordenes que caben en la cola de cocina
const int USUARIOS=3; // cantidad de usuarios del sistema (admin, mesero y cocinero)


string nombreUsuario[USUARIOS]={"admin","mesero","cocinero"};
string claveCifrada[USUARIOS]={"7d04nURC","05s529URC","3934n529URC"};
int rolUsuario[USUARIOS]={1,2,3}; // 1 administrador, 2 mesero, 3 cocinero
int rolActual=0; // rol del usuario que inicio sesion (0 si nadie ha iniciado)
string usuarioActual=""; // nombre del usuario que inicio sesion
int cajaAbierta=0; // pasa a 1 cuando un administrador o mesero abre la caja

// caja actual: billetes y monedas que hay en caja en este momento (cambian con cada cobro en efectivo)
int billetes5=0,billetes10=0,billetes20=0,billetes50=0,billetes100=0; //variables para billetes
int monedas25=0,monedas50=0,monedas1=0; //variables para monedas
// caja de la apertura: copia de lo que se ingreso al iniciar, se usa en el cierre
int apertura5=0,apertura10=0,apertura20=0,apertura50=0,apertura100=0;
int aperturaM25=0,aperturaM50=0,aperturaM1=0;
double sumaBilletes, sumaMonedas;

// productos del menu
string nombreProducto[PRODUCTOS]={"cocacola","pepsi","seven-up","pizza peperoni mediana","pizza jamon mediana","pizza hawaiana"};
double precioProducto[PRODUCTOS]={6,6,5,60,55,65};
int ventasProducto[PRODUCTOS]; // cantidad vendida en el dia de cada producto

// mesas
int sillasMesa[MESAS]={2,2,4,4,6,10}; // para cuantas personas es cada mesa
int estadoMesa[MESAS]; // 0 liberada, 1 ocupada, 2 reservada
string nombreReserva[MESAS]; // nombre de quien reservo cada mesa
int cuentaMesa[MESAS][PRODUCTOS]; // cantidad pedida de cada producto por mesa (la cuenta abierta de cada mesa)

// cola de cocina
int ordenMesa[MAXORDENES]; // mesa a la que pertenece cada orden
int ordenCantidad[MAXORDENES][PRODUCTOS]; // cantidad de cada producto que lleva cada orden
int ordenEntregada[MAXORDENES]; // 0 pendiente, 1 entregada
int totalOrdenes=0; // cuantas ordenes se han tomado en el dia

// acumulados del dia
double totalVendido=0, totalCobrado=0, montoEfectivo=0, montoTarjeta=0;
int totalDescuentos=0, pagosEfectivo=0, pagosTarjeta=0;
int numeroCuenta=0; // cuentas cobradas en el dia
double ultimoRecibido=0, ultimoCambio=0; // datos del ultimo pago en efectivo (para el archivo de la cuenta)

void disenoMenu();
void aperturaCaja();
void estadoMesas();
void reservarMesa();
void tomarOrden();
void generarPrecuenta();
void colaCocina();
int finalizarDia();
int cobrarEfectivo(double totalPagar);
void guardarCuenta(int mesa, double total, int descuento, string descripcion, double totalPagar, int metodoPago, string nombreCliente);
void mostrarCuenta(int mesa);
void mostrarCantidad(string nombre, int cantidad);
void mostrarDejar(string nombre, int dejar, int apertura);
double totalCuenta(int mesa);
int cantidadCambio(double pendiente, double valor, int disponible);
int cantidadMenor(int n1, int n2);
int pedirNumero(string mensaje, int minimo, int maximo);
string pedirPalabra(string mensaje);
string cifrarMurcielago(string texto);
void iniciarSesion();
int puedeUsarOpcion(int opcion);


int main() {
    int opcion;
    int diaTerminado=0; // bandera para terminar el ciclo, pasa a 1 al finalizar el dia
    while (diaTerminado==0){
        iniciarSesion(); // no termina hasta que se ingresen un usuario y una contrasena correctos
        if ((rolActual==1 || rolActual==2) && cajaAbierta==0){ // 1 administrador, 2 mesero
            aperturaCaja(); // la caja se abre una sola vez al iniciar
            cajaAbierta=1;
        }
        opcion=-1; // para que entre al ciclo del menu
        while (opcion!=0 && diaTerminado==0){ // 0 = cerrar sesion
            system("cls");
            disenoMenu();
            opcion=pedirNumero("ingrese el numero correlativo a la opcion que desea probar: ",0,6); // 6 = ultima opcion del menu
            system("cls");
            if (opcion==0){
                cout<<"sesion cerrada"<<endl;
            } else if (puedeUsarOpcion(opcion)==0){
                cout<<"su usuario ("<<usuarioActual<<") no tiene permiso para usar esa opcion"<<endl;
            } else {
                switch (opcion) {
                    case 1:
                        estadoMesas();
                        break;
                    case 2:
                        reservarMesa();
                        break;
                    case 3:
                        tomarOrden();
                        break;
                    case 4:
                        generarPrecuenta();
                        break;
                    case 5:
                        colaCocina();
                        break;
                    case 6:
                        diaTerminado=finalizarDia(); // si no se puede cerrar el dia devuelve 0 y el menu sigue
                        break;
                }
            }
            cout<<endl;
            system("pause");
        }
    }
    cout<<"fin del programa, hasta manana!"<<endl;
    return 0;
}

void iniciarSesion(){
    int intentos=0; // intentos fallidos seguidos
    int i, posicion;
    string usuario, clave;
    time_t inicioEspera;
    rolActual=0;
    while (rolActual==0){
        system("cls");
        cout<<"---------------------- inicio de sesion ------------------------"<<endl;
        usuario=pedirPalabra("usuario: ");
        clave=pedirPalabra("contrasena: ");
        clave=cifrarMurcielago(clave); // se compara cifrada contra cifrada
        posicion=-1; // -1 = no se encontro un usuario con esa contrasena
        for (i=0;i<USUARIOS;i++){
            if (usuario==nombreUsuario[i] && clave==claveCifrada[i]){
                posicion=i;
            }
        }
        if (posicion==-1){
            intentos++;
            cout<<"usuario o contrasena incorrectos (intento "<<intentos<<" de 3)"<<endl; // 3 = intentos permitidos
            if (intentos==3){
                cout<<"demasiados intentos, el login se bloquea por 15 segundos..."<<endl; // 15 = segundos de bloqueo
                inicioEspera=time(NULL);
                while (time(NULL)-inicioEspera<15){ // 15 = segundos de bloqueo
                    // se espera sin hacer nada hasta que pasen los segundos
                }
                intentos=0;
            }
            system("pause");
        } else {
            rolActual=rolUsuario[posicion];
            usuarioActual=nombreUsuario[posicion];
        }
    }
    cout<<"bienvenido "<<usuarioActual<<endl;
    system("pause");
}
// funcion cifrarMurcielago: cambia cada letra de la palabra MURCIELAGO por su numero (M=0 ... O=9)
// y cada numero por su letra. Los demas caracteres quedan igual
string cifrarMurcielago(string texto){
    string clave="MURCIELAGO"; // la posicion de cada letra es su numero
    string resultado=texto;
    int i, j;
    char caracter;
    for (i=0;i<(int)texto.length();i++){
        caracter=texto[i];
        if (caracter>='0' && caracter<='9'){
            resultado[i]=clave[caracter-'0']; // numero -> letra
        } else {
            if (caracter>='a' && caracter<='z'){
                caracter=caracter-32; // se pasa a mayuscula (32 = diferencia entre minuscula y mayuscula en ASCII)
            }
            for (j=0;j<10;j++){ // 10 = letras de MURCIELAGO
                if (caracter==clave[j]){
                    resultado[i]='0'+j; // letra -> numero
                }
            }
        }
    }
    return resultado;
}

int puedeUsarOpcion(int opcion){
    if (rolActual==1){
        return 1;
    }
    if (rolActual==2 && opcion>=1 && opcion<=4){ // 4 = ultima opcion de mesas, ordenes y cobro
        return 1;
    }
    if (rolActual==3 && opcion==5){ // 5 = cola de cocina
        return 1;
    }
    return 0;
}
// funcion disenoMenu: muestra solo las opciones del menu principal que le corresponden al usuario que inicio sesion
void disenoMenu(){
    cout<<endl;
    cout<<"---------------------- menu ------------------------"<<endl;
    cout<<"usuario: "<<usuarioActual<<endl;
    if (rolActual==1 || rolActual==2){ // administrador y mesero
        cout<<"1. Mostrar estado de mesas"<<endl;
        cout<<"2. Reservar mesa"<<endl;
        cout<<"3. Tomar la orden de una mesa"<<endl;
        cout<<"4. Generar precuenta y cobrar"<<endl;
    }
    if (rolActual==1 || rolActual==3){ // administrador y cocinero
        cout<<"5. Mostrar cola de cocina"<<endl;
    }
    if (rolActual==1){ // solo el administrador
        cout<<"6. Finalizar dia"<<endl;
    }
    cout<<"0. Cerrar sesion"<<endl;
}

void aperturaCaja() { //apertura de caja
    int opcionApertura;
    do {
        system("cls");
        cout<<"                                apertura de caja                     "<<endl;
        cout<<"- ingrese la cantidad de billetes que hay en caja:"<<endl;
        billetes5=pedirNumero("\t1. cantidad de billetes de Q5: ",0,10000);
        billetes10=pedirNumero("\t2. cantidad de billetes de Q10: ",0,10000);
        billetes20=pedirNumero("\t3. cantidad de billetes de Q20: ",0,10000);
        billetes50=pedirNumero("\t4. cantidad de billetes de Q50: ",0,10000);
        billetes100=pedirNumero("\t5. cantidad de billetes de Q100: ",0,10000);
        cout<<"\n";
        cout<<"-ingrese la cantidad de monedas que hay en caja: "<<endl;
        monedas25=pedirNumero("\t1. cantidad de monedas de Q0.25: ",0,10000);
        monedas50=pedirNumero("\t2. cantidad de monedas de Q0.50: ",0,10000);
        monedas1=pedirNumero("\t3. cantidad de monedas de Q1: ",0,10000);
        cout<<"\n";
        opcionApertura=pedirNumero("Ha ingresado correctamente los valores? <1> Si / <2> No, volver a ingresarlos: ",1,2);
    }while (opcionApertura!=1);
    cout<<"\n";
    apertura5=billetes5; // guardamos lo que hay en la apertura para usarlo en el cierre
    apertura10=billetes10;
    apertura20=billetes20;
    apertura50=billetes50;
    apertura100=billetes100;
    aperturaM25=monedas25;
    aperturaM50=monedas50;
    aperturaM1=monedas1;
    sumaBilletes=billetes5*5+billetes10*10+billetes20*20+billetes50*50+billetes100*100;
    sumaMonedas=monedas25*0.25+monedas50*0.50+monedas1;
    cout<<"La suma total de los billetes es de: Q"<<sumaBilletes<<endl;
    cout<<"La suma total de las monedas es de: Q"<<sumaMonedas<<endl;
    cout<<"El total de dinero en caja es de: Q"<<sumaBilletes+sumaMonedas<<endl;
    cout<<"\n";
    system("pause");
    system("cls");
}

void estadoMesas() {
    int i;
    cout<<"---------------------- estado de mesas ------------------------"<<endl;
    for (i=0;i<MESAS;i++){
        cout<<"Mesa no."<<i+1<<" | sillas: "<<sillasMesa[i]<<" | estado: ";
        if (estadoMesa[i]==0){
            cout<<"liberada";
        } else if (estadoMesa[i]==1){
            cout<<"ocupada | cuenta actual: Q"<<totalCuenta(i);
        } else {
            cout<<"reservada a nombre de "<<nombreReserva[i];
        }
        cout<<endl;
    }
}

void reservarMesa(){
    int personas, i, mesaElegida, disponibles=0, mesaValida;
    cout<<"---------------------- reservar mesa ------------------------"<<endl;
    personas=pedirNumero("para cuantas personas necesita mesa: ",1,100); // 100 = limite razonable de personas en una reserva
    cout<<"mesas disponibles para "<<personas<<" personas:"<<endl;
    for (i=0;i<MESAS;i++){
        if (estadoMesa[i]==0 && sillasMesa[i]>=personas){ // liberada y con sillas suficientes
            cout<<"\tMesa no."<<i+1<<" ("<<sillasMesa[i]<<" sillas)"<<endl;
            disponibles++;
        }
    }
    if (disponibles==0){
        cout<<"no hay mesas disponibles para esa cantidad de personas"<<endl;
    } else {
        do{ // validamos que la mesa elegida este en la lista de disponibles
            mesaElegida=pedirNumero("ingrese el numero de la mesa que desea reservar (0 para cancelar): ",0,MESAS);
            mesaValida=1;
            if (mesaElegida!=0){
                if (estadoMesa[mesaElegida-1]!=0 || sillasMesa[mesaElegida-1]<personas){
                    cout<<"la mesa elegida no esta en la lista de mesas disponibles"<<endl;
                    mesaValida=0;
                }
            }
        }while (mesaValida==0);
        if (mesaElegida!=0){
            nombreReserva[mesaElegida-1]=pedirPalabra("ingrese el nombre de quien reserva (una sola palabra): ");
            estadoMesa[mesaElegida-1]=2; // la mesa queda reservada
            cout<<"la mesa no."<<mesaElegida<<" ha sido reservada a nombre de "<<nombreReserva[mesaElegida-1]<<endl;
        }
    }
}

void tomarOrden(){
    int mesa, i, totalProductos=0;
    int nuevoPedido[PRODUCTOS]; // lo que se pide en esta orden
    cout<<"---------------------- tomar orden ------------------------"<<endl;
    estadoMesas();
    cout<<endl;
    mesa=pedirNumero("ingrese el numero de la mesa (0 para cancelar): ",0,MESAS);
    if (mesa==0){
        return;
    }
    mesa=mesa-1; // los arreglos empiezan en 0
    if (totalOrdenes>=MAXORDENES){ // validamos que quepa en la cola de cocina
        cout<<"la cola de cocina esta llena, no se pueden tomar mas ordenes"<<endl;
        return;
    }
    cout<<endl;
    cout<<"menu del restaurante:"<<endl;
    for (i=0;i<PRODUCTOS;i++){
        cout<<"\t"<<i+1<<". "<<nombreProducto[i]<<" - Q"<<precioProducto[i]<<endl;
    }
    cout<<endl;
    for (i=0;i<PRODUCTOS;i++){ // pedimos la cantidad de cada producto
        nuevoPedido[i]=pedirNumero("cantidad de "+nombreProducto[i]+" que desea agregar (0 si no desea): ",0,100);
        totalProductos+=nuevoPedido[i];
    }
    if (totalProductos==0){
        cout<<"no se agrego ningun producto a la mesa"<<endl;
    } else {
        for (i=0;i<PRODUCTOS;i++){
            cuentaMesa[mesa][i]+=nuevoPedido[i]; // se suma a la cuenta de la mesa
            ordenCantidad[totalOrdenes][i]=nuevoPedido[i]; // y pasa a la cola de cocina
        }
        ordenMesa[totalOrdenes]=mesa;
        ordenEntregada[totalOrdenes]=0;
        totalOrdenes++;
        estadoMesa[mesa]=1; // la mesa queda ocupada
        cout<<endl;
        cout<<"orden agregada, cuenta actual de la mesa no."<<mesa+1<<":"<<endl;
        mostrarCuenta(mesa);
    }
}

void generarPrecuenta(){
    int mesa, i, opcion, descuento, metodoPago, pagoRealizado;
    double totalCuentaMesa, totalPagar;
    string descripcionDescuento, nombreCliente;
    cout<<"---------------------- generar precuenta ------------------------"<<endl;
    estadoMesas();
    cout<<endl;
    mesa=pedirNumero("ingrese el numero de la mesa (0 para cancelar): ",0,MESAS);
    if (mesa==0){
        return;
    }
    mesa=mesa-1;
    if (estadoMesa[mesa]!=1){ // solo se puede hacer precuenta de una mesa con cuenta abierta
        cout<<"la mesa no."<<mesa+1<<" no tiene una cuenta abierta"<<endl;
        return;
    }
    cout<<endl;
    cout<<"precuenta de la mesa no."<<mesa+1<<":"<<endl;
    mostrarCuenta(mesa);
    totalCuentaMesa=totalCuenta(mesa);
    opcion=pedirNumero("la cuenta es correcta? <1> Si, cobrar / <2> No, volver al menu: ",1,2);
    if (opcion==2){
        cout<<"la cuenta de la mesa sigue abierta, puede agregar productos desde la opcion 3"<<endl;
        return;
    }
    // descuento: un solo descuento por cuenta, en quetzales, con su descripcion
    descuento=0;
    descripcionDescuento="";
    opcion=pedirNumero("se aplicara un descuento? <1> Si / <2> No: ",1,2);
    if (opcion==1){
        descripcionDescuento=pedirPalabra("ingrese la descripcion del descuento (una sola palabra): ");
        cout<<"el descuento maximo es de Q"<<totalCuentaMesa<<endl; // el descuento no puede ser mayor a la cuenta
        descuento=pedirNumero("ingrese el monto del descuento en quetzales: ",1,totalCuentaMesa);
    }
    totalPagar=totalCuentaMesa-descuento;
    cout<<"total a pagar: Q"<<totalPagar<<endl;
    // cobro: si el efectivo no se puede completar se vuelve a preguntar la forma de pago
    metodoPago=0; // 0 sin cobro, 1 efectivo, 2 tarjeta
    if (totalPagar>0){
        do{
            metodoPago=pedirNumero("forma de pago <1> efectivo / <2> tarjeta: ",1,2);
            if (metodoPago==1){
                pagoRealizado=cobrarEfectivo(totalPagar);
            } else {
                pagoRealizado=1;
            }
        }while (pagoRealizado==0);
    } else {
        cout<<"la cuenta queda en Q0, no se realiza ningun cobro"<<endl;
    }
    numeroCuenta++; // acumulamos los valores del dia para el cierre
    totalVendido+=totalCuentaMesa;
    totalDescuentos+=descuento;
    totalCobrado+=totalPagar;
    for (i=0;i<PRODUCTOS;i++){
        ventasProducto[i]+=cuentaMesa[mesa][i];
    }
    if (metodoPago==1){
        pagosEfectivo++;
        montoEfectivo+=totalPagar;
    } else if (metodoPago==2){
        pagosTarjeta++;
        montoTarjeta+=totalPagar;
    }
    nombreCliente=pedirPalabra("ingrese una palabra para el nombre del archivo de la cuenta (por ejemplo el nombre del cliente): ");
    guardarCuenta(mesa,totalCuentaMesa,descuento,descripcionDescuento,totalPagar,metodoPago,nombreCliente);
    for (i=0;i<PRODUCTOS;i++){
        cuentaMesa[mesa][i]=0; // se limpia la cuenta y se libera la mesa
    }
    estadoMesa[mesa]=0;
    cout<<"la mesa no."<<mesa+1<<" ha sido liberada"<<endl;
}

int cobrarEfectivo(double totalPagar){
    int recibido5, recibido10, recibido20, recibido50, recibido100, recibidoM25, recibidoM50, recibidoM1; // lo que entrega el cliente
    int disp5, disp10, disp20, disp50, disp100, dispM25, dispM50, dispM1; // lo que hay en caja mas lo que entrega el cliente
    int dar5, dar10, dar20, dar50, dar100, darM25, darM50, darM1; // lo que se da de cambio
    double recibidoTotal, cambio, pendiente;
    int opcion, cambioListo;
    do{
        cout<<"ingrese los billetes y monedas que entrega el cliente:"<<endl;
        recibido5=pedirNumero("\tcantidad de billetes de Q5: ",0,10000);
        recibido10=pedirNumero("\tcantidad de billetes de Q10: ",0,10000);
        recibido20=pedirNumero("\tcantidad de billetes de Q20: ",0,10000);
        recibido50=pedirNumero("\tcantidad de billetes de Q50: ",0,10000);
        recibido100=pedirNumero("\tcantidad de billetes de Q100: ",0,10000);
        recibidoM25=pedirNumero("\tcantidad de monedas de Q0.25: ",0,10000);
        recibidoM50=pedirNumero("\tcantidad de monedas de Q0.50: ",0,10000);
        recibidoM1=pedirNumero("\tcantidad de monedas de Q1: ",0,10000);
        recibidoTotal=recibido5*5+recibido10*10+recibido20*20+recibido50*50+recibido100*100+recibidoM25*0.25+recibidoM50*0.50+recibidoM1;
        cambioListo=0; // pasa a 1 cuando se logra dar el cambio exacto
        if (recibidoTotal<totalPagar){ // validamos que alcance para pagar
            cout<<"el cliente entrego Q"<<recibidoTotal<<" y el total a pagar es Q"<<totalPagar<<", no alcanza"<<endl;
        } else {
            cambio=recibidoTotal-totalPagar;
            pendiente=cambio; // lo que falta por dar de cambio
            disp5=billetes5+recibido5; // el cliente tambien deja sus billetes y monedas en caja
            disp10=billetes10+recibido10;
            disp20=billetes20+recibido20;
            disp50=billetes50+recibido50;
            disp100=billetes100+recibido100;
            dispM25=monedas25+recibidoM25;
            dispM50=monedas50+recibidoM50;
            dispM1=monedas1+recibidoM1;
            // damos el cambio de mayor a menor valor, restando lo que se va dando
            dar100=cantidadCambio(pendiente,100,disp100);
            pendiente=pendiente-dar100*100;
            dar50=cantidadCambio(pendiente,50,disp50);
            pendiente=pendiente-dar50*50;
            dar20=cantidadCambio(pendiente,20,disp20);
            pendiente=pendiente-dar20*20;
            dar10=cantidadCambio(pendiente,10,disp10);
            pendiente=pendiente-dar10*10;
            dar5=cantidadCambio(pendiente,5,disp5);
            pendiente=pendiente-dar5*5;
            darM1=cantidadCambio(pendiente,1,dispM1);
            pendiente=pendiente-darM1*1;
            darM50=cantidadCambio(pendiente,0.50,dispM50);
            pendiente=pendiente-darM50*0.50;
            darM25=cantidadCambio(pendiente,0.25,dispM25);
            pendiente=pendiente-darM25*0.25;
            if (pendiente>0){ // si sobro algo, la caja no puede dar el cambio exacto
                cout<<"la caja no tiene billetes y monedas para dar el cambio exacto de Q"<<cambio<<endl;
            } else {
                billetes5=disp5-dar5; // actualizamos la caja
                billetes10=disp10-dar10;
                billetes20=disp20-dar20;
                billetes50=disp50-dar50;
                billetes100=disp100-dar100;
                monedas25=dispM25-darM25;
                monedas50=dispM50-darM50;
                monedas1=dispM1-darM1;
                cout<<"cambio a entregar: Q"<<cambio<<endl;
                mostrarCantidad("billetes de Q100",dar100);
                mostrarCantidad("billetes de Q50",dar50);
                mostrarCantidad("billetes de Q20",dar20);
                mostrarCantidad("billetes de Q10",dar10);
                mostrarCantidad("billetes de Q5",dar5);
                mostrarCantidad("monedas de Q1",darM1);
                mostrarCantidad("monedas de Q0.50",darM50);
                mostrarCantidad("monedas de Q0.25",darM25);
                ultimoRecibido=recibidoTotal; // se guardan para el archivo de la cuenta
                ultimoCambio=cambio;
                cambioListo=1;
            }
        }
        if (cambioListo==0){ // si no se pudo cobrar, se pregunta que hacer
            opcion=pedirNumero("<1> volver a ingresar lo que entrega el cliente / <2> cambiar forma de pago: ",1,2);
            if (opcion==2){
                return 0;
            }
        }
    }while (cambioListo==0);
    return 1;
}
// funcion cantidadCambio: devuelve cuantos billetes o monedas de un valor se pueden dar de cambio,
// son los que caben en lo pendiente, pero sin pasar de los que hay disponibles
int cantidadCambio(double pendiente, double valor, int disponible){
    int cantidad;
    cantidad=pendiente/valor; // al guardarlo en un entero se quitan los decimales
    if (cantidad>disponible){
        cantidad=disponible;
    }
    return cantidad;
}
// funcion mostrarCantidad: muestra una linea del cambio, solo si la cantidad es mayor a 0
void mostrarCantidad(string nombre, int cantidad){
    if (cantidad>0){
        cout<<"\t"<<cantidad<<" "<<nombre<<endl;
    }
}
// funcion guardarCuenta: crea el archivo .txt de la cuenta final de una mesa
// (el nombre es la palabra ingresada mas .txt) con los productos, el total, el descuento y la forma de pago
void guardarCuenta(int mesa, double total, int descuento, string descripcion, double totalPagar, int metodoPago, string nombreCliente){
    int i;
    ofstream archivo(nombreCliente+".txt");
    archivo<<"cuenta final - mesa no."<<mesa+1<<endl;
    archivo<<"cuenta no. "<<numeroCuenta<<endl;
    archivo<<endl;
    for (i=0;i<PRODUCTOS;i++){ // solo escribimos los productos que si se pidieron
        if (cuentaMesa[mesa][i]>0){
            archivo<<cuentaMesa[mesa][i]<<" x "<<nombreProducto[i]<<" (Q"<<precioProducto[i]<<" c/u) = Q"<<cuentaMesa[mesa][i]*precioProducto[i]<<endl;
        }
    }
    archivo<<endl;
    archivo<<"total de la cuenta: Q"<<total<<endl;
    if (descuento>0){ // la descripcion del descuento solo va en el archivo
        archivo<<"descuento: Q"<<descuento<<endl;
        archivo<<"descripcion del descuento: "<<descripcion<<endl;
    }
    archivo<<"total pagado: Q"<<totalPagar<<endl;
    if (metodoPago==1){
        archivo<<"forma de pago: efectivo"<<endl;
        archivo<<"efectivo recibido: Q"<<ultimoRecibido<<endl;
        archivo<<"cambio entregado: Q"<<ultimoCambio<<endl;
    } else if (metodoPago==2){
        archivo<<"forma de pago: tarjeta"<<endl;
    } else {
        archivo<<"forma de pago: sin cobro"<<endl;
    }
    archivo.close();
    cout<<"se genero el archivo "<<nombreCliente<<".txt"<<endl;
}

    int i, j, pendientes, orden, ordenValida;
    do{
        pendientes=0;
        cout<<"---------------------- cola de cocina ------------------------"<<endl;
        for (i=0;i<totalOrdenes;i++){
            if (ordenEntregada[i]==0){ // solo mostramos las pendientes
                pendientes++;
                cout<<"Orden no."<<i+1<<" - mesa no."<<ordenMesa[i]+1<<": ";
                for (j=0;j<PRODUCTOS;j++){
                    if (ordenCantidad[i][j]>0){
                        cout<<ordenCantidad[i][j]<<" x "<<nombreProducto[j]<<"   ";
                    }
                }
                cout<<endl;
            }
        }
        if (pendientes==0){
            cout<<"no hay ordenes pendientes en cocina"<<endl;
            orden=0;
        } else {
            do{
                orden=pedirNumero("ingrese el numero de la orden que ya se entrego (0 para volver al menu): ",0,totalOrdenes);
                ordenValida=1;
                if (orden!=0 && ordenEntregada[orden-1]==1){ // validamos que no este entregada
                    cout<<"esa orden ya fue entregada"<<endl;
                    ordenValida=0;
                }
            }while (ordenValida==0);
            if (orden!=0){
                ordenEntregada[orden-1]=1;
                cout<<"la orden no."<<orden<<" ha sido marcada como entregada"<<endl<<endl;
            }
        }
    }while (orden!=0);
}
// funcion finalizarDia: primero verifica que no haya mesas ocupadas ni ordenes sin entregar.
// Si todo esta resuelto y se confirma, muestra el cierre de caja (ventas, descuentos, pagos en efectivo
// y con tarjeta, billetes y monedas que se dejan para el dia siguiente y el efectivo que se lleva a casa)
// y devuelve 1 para que el programa termine. Si no se puede cerrar devuelve 0.
int finalizarDia(){
    int i, opcion, ocupadas=0, pendientes=0;
    int dejar5, dejar10, dejar20, dejar50, dejar100, dejarM25, dejarM50, dejarM1; // lo que se deja en caja
    double totalApertura, totalCaja, totalDejar;
    cout<<"---------------------- finalizar dia ------------------------"<<endl;
    for (i=0;i<MESAS;i++){ // contamos las mesas con cuenta sin cobrar
        if (estadoMesa[i]==1){
            ocupadas++;
        }
    }
    for (i=0;i<totalOrdenes;i++){ // contamos las ordenes sin entregar
        if (ordenEntregada[i]==0){
            pendientes++;
        }
    }
    if (ocupadas>0 || pendientes>0){ // no se puede cerrar con cosas sin resolver
        cout<<"no se puede finalizar el dia todavia:"<<endl;
        if (ocupadas>0){
            cout<<"- hay "<<ocupadas<<" mesa(s) con cuenta sin cobrar"<<endl;
        }
        if (pendientes>0){
            cout<<"- hay "<<pendientes<<" orden(es) sin entregar en cocina"<<endl;
        }
        return 0;
    }
    opcion=pedirNumero("esta seguro que desea finalizar el dia? <1> Si / <2> No: ",1,2);
    if (opcion==2){
        return 0;
    }
    system("cls");
    cout<<"---------------------- cierre de caja ------------------------"<<endl;
    cout<<"- productos vendidos en el dia:"<<endl;
    for (i=0;i<PRODUCTOS;i++){
        if (ventasProducto[i]>0){
            cout<<"\t"<<ventasProducto[i]<<" x "<<nombreProducto[i]<<" = Q"<<ventasProducto[i]*precioProducto[i]<<endl;
        }
    }
    if (numeroCuenta==0){
        cout<<"\tno se vendio ningun producto"<<endl;
    }
    cout<<endl;
    cout<<"cuentas cobradas: "<<numeroCuenta<<endl;
    cout<<"total vendido: Q"<<totalVendido<<endl;
    cout<<"total de descuentos: Q"<<totalDescuentos<<endl;
    cout<<"total cobrado: Q"<<totalCobrado<<endl;
    cout<<endl;
    cout<<"pagos en efectivo: "<<pagosEfectivo<<" veces por un total de Q"<<montoEfectivo<<endl;
    cout<<"pagos con tarjeta: "<<pagosTarjeta<<" veces por un total de Q"<<montoTarjeta<<endl;
    cout<<endl;
    // sumamos lo que habia al abrir la caja y lo que hay ahora
    totalApertura=apertura5*5+apertura10*10+apertura20*20+apertura50*50+apertura100*100+aperturaM25*0.25+aperturaM50*0.50+aperturaM1;
    totalCaja=billetes5*5+billetes10*10+billetes20*20+billetes50*50+billetes100*100+monedas25*0.25+monedas50*0.50+monedas1;
    cout<<"dinero con el que se abrio la caja: Q"<<totalApertura<<endl;
    cout<<"efectivo en caja al cierre: Q"<<totalCaja<<endl;
    cout<<endl;
    // se deja la misma cantidad de la apertura, o lo que haya en caja si hay menos
    dejar5=cantidadMenor(billetes5,apertura5);
    dejar10=cantidadMenor(billetes10,apertura10);
    dejar20=cantidadMenor(billetes20,apertura20);
    dejar50=cantidadMenor(billetes50,apertura50);
    dejar100=cantidadMenor(billetes100,apertura100);
    dejarM25=cantidadMenor(monedas25,aperturaM25);
    dejarM50=cantidadMenor(monedas50,aperturaM50);
    dejarM1=cantidadMenor(monedas1,aperturaM1);
    cout<<"- billetes y monedas que se dejan para la apertura del dia siguiente:"<<endl;
    mostrarDejar("billetes de Q5",dejar5,apertura5);
    mostrarDejar("billetes de Q10",dejar10,apertura10);
    mostrarDejar("billetes de Q20",dejar20,apertura20);
    mostrarDejar("billetes de Q50",dejar50,apertura50);
    mostrarDejar("billetes de Q100",dejar100,apertura100);
    mostrarDejar("monedas de Q0.25",dejarM25,aperturaM25);
    mostrarDejar("monedas de Q0.50",dejarM50,aperturaM50);
    mostrarDejar("monedas de Q1",dejarM1,aperturaM1);
    totalDejar=dejar5*5+dejar10*10+dejar20*20+dejar50*50+dejar100*100+dejarM25*0.25+dejarM50*0.50+dejarM1;
    cout<<"total que se deja en caja: Q"<<totalDejar<<endl;
    cout<<"efectivo que se lleva a casa: Q"<<totalCaja-totalDejar<<endl; // todo lo que no se deja en caja
    return 1;
}
// funcion cantidadMenor: devuelve el menor de dos numeros
int cantidadMenor(int n1, int n2){
    if (n1<n2){
        return n1;
    }
    return n2;
}
// funcion mostrarDejar: muestra una linea del cierre con lo que se deja en caja de un billete o moneda
// y avisa cuantos faltan respecto a la apertura (si se uso para dar cambio)
void mostrarDejar(string nombre, int dejar, int apertura){
    cout<<"\t"<<dejar<<" "<<nombre;
    if (dejar<apertura){
        cout<<" (faltan "<<apertura-dejar<<" respecto a la apertura)";
    }
    cout<<endl;
}
// funcion mostrarCuenta: muestra los productos de la cuenta abierta de una mesa con su subtotal y el total
void mostrarCuenta(int mesa){
    int i;
    for (i=0;i<PRODUCTOS;i++){
        if (cuentaMesa[mesa][i]>0){
            cout<<"\t"<<cuentaMesa[mesa][i]<<" x "<<nombreProducto[i]<<" (Q"<<precioProducto[i]<<" c/u) = Q"<<cuentaMesa[mesa][i]*precioProducto[i]<<endl;
        }
    }
    cout<<"\ttotal de la cuenta: Q"<<totalCuenta(mesa)<<endl;
}
// funcion totalCuenta: devuelve el total de la cuenta de una mesa
// (suma cantidad por precio de cada producto)
double totalCuenta(int mesa){
    int i;
    double total=0;
    for (i=0;i<PRODUCTOS;i++){
        total+=cuentaMesa[mesa][i]*precioProducto[i];
    }
    return total;
}
// funcion pedirNumero: muestra un mensaje y lee un numero entero, repitiendo la pregunta mientras
// lo escrito no sea un numero (si hay letras, espacios o simbolos) o este fuera del rango (minimo y maximo).
// El dato se lee como texto con getline y se revisa letra por letra, asi el programa no falla si escriben letras
int pedirNumero(string mensaje, int minimo, int maximo){
    int numero=0, i, valido;
    string texto;
    do{
        valido=1; // pasa a 0 si algo esta mal
        cout<<mensaje;
        getline(cin,texto);
        if (texto.length()==0){
            cout<<"debe ingresar un numero, no puede dejarlo vacio"<<endl;
            valido=0;
        } else if (texto.length()>9){ // 9 = digitos maximos para que el numero quepa en un int
            cout<<"el numero ingresado es demasiado grande"<<endl;
            valido=0;
        } else {
            for (i=0;i<(int)texto.length();i++){ // revisamos que todo sean digitos
                if (texto[i]<'0' || texto[i]>'9'){
                    valido=0;
                }
            }
            if (valido==0){
                cout<<"debe ingresar numeros enteros (sin letras, espacios ni simbolos)"<<endl;
            }
        }
        if (valido==1){
            numero=0;
            for (i=0;i<(int)texto.length();i++){ // convertimos el texto a numero digito por digito
                numero=numero*10+(texto[i]-'0');
            }
            if (numero<minimo || numero>maximo){
                cout<<"el numero ingresado debe estar entre "<<minimo<<" y "<<maximo<<endl;
                valido=0;
            }
        }
    }while (valido==0);
    return numero;
}
// funcion pedirPalabra: muestra un mensaje y lee una sola palabra (con getline), repitiendo la pregunta
// si se deja vacia, si tiene espacios o si tiene mas de 20 letras
string pedirPalabra(string mensaje){
    int i, valido;
    string texto;
    do{
        valido=1;
        cout<<mensaje;
        getline(cin,texto);
        if (texto.length()==0){
            cout<<"no puede dejar este dato vacio"<<endl;
            valido=0;
        } else if (texto.length()>20){ // 20 = largo maximo permitido
            cout<<"el dato no puede tener mas de 20 caracteres"<<endl;
            valido=0;
        } else {
            for (i=0;i<(int)texto.length();i++){
                if (texto[i]==' '){
                    valido=0;
                }
            }
            if (valido==0){
                cout<<"el dato no puede tener espacios"<<endl;
            }
        }
    }while (valido==0);
    return texto;
}