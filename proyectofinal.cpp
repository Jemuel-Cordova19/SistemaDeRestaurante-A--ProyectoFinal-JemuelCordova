//Jemuel Josue Esteban Cordova Huales
// 2294/26/9426
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib> // system y exit
#include <cstdio>  // remove y rename (para borrar y renombrar archivos)
#include <ctime>   // fecha y hora
#include <cctype>  // tolower
#include <thread>  // sleep_for (espera del bloqueo del login)
#include <chrono>  // segundos de espera
#ifdef _WIN32
#include <conio.h> // _getch (para ocultar la contrasena con asteriscos en windows)
#endif
using namespace std;

// poner en 0 si la contrasena no se comporta bien en la consola de clion (entonces se muestra normal)
#define OCULTAR_CONTRASENA 1

// ---------------------------------------------------------------------
// usuarios y sesion
// ---------------------------------------------------------------------
int cantidadUsuarios=3; // cuantos usuarios hay en las listas de abajo
string nombreUsuario[3]={"admin","cajero1","mesero1"}; // 3 = cantidad de usuarios fijos
string nombreCompletoUsuario[3]={"Administrador","Cajero uno","Mesero uno"}; // 3 = cantidad de usuarios fijos
int rolUsuario[3]={1,2,3}; // 3 = cantidad de usuarios fijos (1 administrador, 2 cajero, 3 mesero)
string contrasenaCifradaUsuario[3]={"7d04n2026","37j72026","05s72026"}; // 3 = cantidad de usuarios fijos
string nombreRol[4]={"sin rol","administrador","cajero","mesero"}; // 4 = posiciones 0 a 3 (la 0 no se usa)
int usuarioActual=-1;   // posicion del usuario que inicio sesion (-1 si nadie)
int rolActual=0;        // rol del usuario que inicio sesion
int sesionAbierta=0;    // 1 mientras hay una sesion iniciada
int diaFinalizado=0;    // pasa a 1 cuando se finaliza el dia y el programa debe terminar
int maximoIntentosLogin=3;    // intentos fallidos permitidos antes de bloquear
int segundosBloqueoLogin=30;  // segundos que dura el bloqueo del login

// ---------------------------------------------------------------------
// menu de productos (se guarda en menu.txt)
// ---------------------------------------------------------------------
int cantidadProductosMenu=0;      // cuantos productos hay en el menu
string nombreProducto[40];        // 40 = maximo de productos que puede tener el menu
int precioProductoCentavos[40];   // 40 = maximo de productos (el precio se guarda en centavos: Q6 = 600)
int categoriaProducto[40];        // 40 = maximo de productos (1 bebidas, 2 pizzas, 3 otros)
int productoAgotado[40];          // 40 = maximo de productos (1 si esta agotado)
int productoActivo[40];           // 40 = maximo de productos (0 si fue desactivado del menu)
int cantidadCategorias=3;         // bebidas, pizzas y otros
string nombreCategoria[3]={"BEBIDAS","PIZZAS","OTROS"}; // 3 = cantidad de categorias

// ---------------------------------------------------------------------
// cuentas: de la 0 a la 5 son las mesas y de la 6 a la 15 son ordenes para llevar
// ---------------------------------------------------------------------
int cantidadMesas=6;              // mesas del restaurante
int cantidadCuentasLlevar=10;     // ordenes para llevar que pueden estar abiertas al mismo tiempo
int cantidadCuentas=16;           // mesas mas ordenes para llevar
int sillasMesa[6]={2,2,4,4,6,10}; // 6 = cantidad de mesas (para cuantas personas es cada mesa)
int estadoCuenta[16];             // 16 = cuentas (0 libre, 1 ocupada o abierta, 2 reservada)
string nombreCuenta[16];          // 16 = cuentas (nombre de quien reservo o del cliente para llevar)
int productosEnCuenta[16][40];    // 16 cuentas x 40 productos: cuantas unidades de cada producto tiene cada cuenta

// ---------------------------------------------------------------------
// cola de preparacion (cocina y barra)
// ---------------------------------------------------------------------
int capacidadCola=200;            // maximo de pedidos que caben en la cola en un dia
int totalEntradasCola=0;          // pedidos enviados a cocina o barra en el dia
int colaCuenta[200];              // 200 = capacidad de la cola (cuenta a la que pertenece cada pedido)
int colaDestino[200];             // 200 = capacidad de la cola (1 cocina, 2 barra)
time_t colaHoraTomada[200];       // 200 = capacidad de la cola (hora en que se tomo el pedido)
int colaEntregada[200];           // 200 = capacidad de la cola (0 pendiente, 1 entregado)
int colaCantidad[200][40];        // 200 pedidos x 40 productos: unidades de cada producto en cada pedido

// ---------------------------------------------------------------------
// caja (billetes y monedas)
// ---------------------------------------------------------------------
int cantidadDenominaciones=8;     // 5 tipos de billetes y 3 tipos de monedas
int valorDenominacionCentavos[8]={500,1000,2000,5000,10000,25,50,100}; // 8 = denominaciones (valor de cada una en centavos)
string nombreDenominacion[8]={"billetes de Q5","billetes de Q10","billetes de Q20","billetes de Q50","billetes de Q100","monedas de Q0.25","monedas de Q0.50","monedas de Q1"}; // 8 = denominaciones
int ordenDeCambio[8]={4,3,2,1,0,7,6,5}; // 8 = denominaciones (posiciones de mayor a menor valor para dar el cambio)
int cajaActual[8];                // 8 = denominaciones (lo que hay en caja en este momento)
int cajaApertura[8];              // 8 = denominaciones (lo que habia al abrir la caja)
int cajaAbierta=0;                // 1 cuando ya se abrio la caja del dia
string usuarioApertura="";        // usuario que abrio la caja
string horaApertura="";           // hora en que se abrio la caja

// retiros y gastos de caja
int cantidadMovimientosCaja=0;           // movimientos registrados en el dia
int movimientoTipo[50];                  // 50 = maximo de movimientos al dia (1 retiro, 2 gasto)
string movimientoDescripcion[50];        // 50 = maximo de movimientos al dia
int movimientoMontoCentavos[50];         // 50 = maximo de movimientos al dia
string movimientoUsuario[50];            // 50 = maximo de movimientos al dia
string movimientoHora[50];               // 50 = maximo de movimientos al dia

// productos anulados
string registroAnulacion[50];            // 50 = maximo de anulaciones al dia (texto con el detalle de cada una)
int cantidadRegistrosAnulacion=0;        // anulaciones registradas en el dia

// ---------------------------------------------------------------------
// acumulados del dia (todo el dinero en centavos)
// ---------------------------------------------------------------------
int totalVentaBrutaCentavos=0;     // suma de las cuentas antes de descuentos
int totalDescuentosCentavos=0;     // descuentos aplicados
int totalVentaNetaCentavos=0;      // venta bruta menos descuentos
int totalPropinasCentavos=0;       // propinas recibidas
int totalIvaIncluidoCentavos=0;    // IVA que ya viene incluido en la venta neta
int totalCobradoCentavos=0;        // venta neta mas propinas
int cantidadCuentasCobradas=0;     // cuentas cobradas en el dia
int pagosEfectivoCantidad=0;       // veces que se pago en efectivo
int pagosEfectivoMontoCentavos=0;  // dinero cobrado en efectivo
int pagosTarjetaCantidad=0;        // veces que se pago con tarjeta
int pagosTarjetaMontoCentavos=0;   // dinero cobrado con tarjeta
int totalRetirosCentavos=0;        // dinero retirado de la caja
int totalGastosCentavos=0;         // gastos pagados desde la caja
int productosAnuladosCantidad=0;   // unidades anuladas
int productosAnuladosMontoCentavos=0; // dinero de las unidades anuladas
int unidadesVendidasHoy[40];       // 40 = maximo de productos (unidades vendidas de cada producto)
int numeroSiguienteCuenta=1;       // numero que tendra la proxima cuenta cobrada (no se reinicia cada dia)

// datos del cobro que se esta realizando (se usan para armar el archivo de la cuenta)
int cobroNumeroCuenta=0;
int cobroCuentaIndice=0;
string cobroNit="";
string cobroNombreCliente="";
int cobroSubtotalCentavos=0;
int cobroDescuentoCentavos=0;
string cobroDescripcionDescuento="";
int cobroIvaCentavos=0;
int cobroPropinaCentavos=0;
int cobroTotalCentavos=0;
int cobroCantidadPagos=0;
int cobroMetodoPago[20];           // 20 = maximo de personas entre las que se puede dividir una cuenta
int cobroMontoPago[20];            // 20 = maximo de personas entre las que se puede dividir una cuenta
int cobroRecibidoPago[20];         // 20 = maximo de personas entre las que se puede dividir una cuenta
int cobroCambioPago[20];           // 20 = maximo de personas entre las que se puede dividir una cuenta
int ultimoRecibidoCentavos=0;      // efectivo recibido en el ultimo pago en efectivo
int ultimoCambioCentavos=0;        // cambio entregado en el ultimo pago en efectivo

// datos del cierre del dia (se usan para armar el reporte)
int cierreConteoCaja[8];           // 8 = denominaciones (lo que conto el cajero)
int cierreDejarEnCaja[8];          // 8 = denominaciones (lo que se deja para el dia siguiente)
int cierreEsperadoCentavos=0;      // efectivo que el sistema dice que debe haber
int cierreContadoCentavos=0;       // efectivo que conto el cajero
int cierreEfectivoCasaCentavos=0;  // efectivo que se lleva a casa
string usuarioCierre="";           // usuario que cerro el dia
string horaCierre="";              // hora del cierre


// entrada de datos con validacion (anti-tontos)
string leerLinea(string mensaje);
string recortarEspacios(string texto);
string aMinusculas(string texto);
long long textoALargo(string texto);
int textoAEntero(string texto);
int leerEntero(string mensaje, int minimo, int maximo);
int leerMontoCentavos(string mensaje, int minimoCentavos, int maximoCentavos, int multiploCentavos);
string leerTextoSinEspacios(string mensaje, int largoMinimo, int largoMaximo);
string leerTextoConEspacios(string mensaje, int largoMinimo, int largoMaximo);
string leerContrasena(string mensaje);
int esNitValido(string nit);
// formatos
string dinero(int centavos);
string numeroConCeros(int numero, int cantidadDigitos);
string formatearFechaHora(time_t momento, string formato);
// login y permisos
string cifrarMurcielago(string texto);
int buscarUsuario(string usuarioBuscado);
void iniciarSesion();
int autorizarComoAdministrador(string motivo);
int tienePermiso(int rolMaximoPermitido);
// pantallas y menus
void mostrarEncabezado();
void mostrarOpcionesMenuPrincipal();
void menuMesas();
void menuCaja();
void administrarMenu();
// productos
void agregarProductoAlMenu(string nombre, int precioCentavos, int categoria);
void crearMenuPorDefecto();
void mostrarMenuProductos(int mostrarOcultos);
int productoEnUsoEnCuentas(int producto);
// cuentas y mesas
string etiquetaCuenta(int cuenta);
int totalCuentaCentavos(int cuenta);
int hayProductosEnCuenta(int cuenta);
void mostrarCuenta(int cuenta);
void mostrarEstadoCuentas();
int elegirCuenta(string mensaje, int filtro);
void reservarMesa();
void cancelarReserva();
void sentarClientes();
void moverCuenta();
void liberarMesaSinConsumo();
// ordenes
void mostrarPedidoNuevo(int pedido[]);
void enviarPedido(int cuenta, int pedido[]);
void tomarOrden();
void anularProducto();
// cobro
void generarPrecuentaYCobrar();
int cobrarEfectivo(int montoCentavos);
void guardarCuentaFinal();
// cola de cocina y barra
void menuCola();
void atenderColaDestino(int destino);
// caja
int sumaDenominaciones(int cantidades[]);
void aperturaCaja();
void mostrarEstadoCaja();
void registrarMovimientoCaja();
// cierre del dia
void escribirReporteCierre(ostream &salida);
void finalizarDia();
// archivos
string leerLineaArchivo(ifstream &archivo);
int separarNumeros(string linea, long long destino[], int maximo);
void cargarMenu();
void guardarMenu();
void cargarContadorCuentas();
void guardarContadorCuentas();
void guardarEstado();
void cargarEstado();

// =====================================================================
// ENTRADA DE DATOS CON VALIDACION (ANTI-TONTOS)
// todas las preguntas del programa pasan por estas funciones, asi nada se
// rompe si el usuario escribe letras, deja vacio o se pasa del rango
// =====================================================================

// funcion leerLinea: muestra un mensaje y lee una linea completa del teclado.
// si la entrada se cierra (por ejemplo se cerro la ventana) guarda el estado y termina
// el programa, para que no se quede repitiendo la pregunta para siempre
string leerLinea(string mensaje){
    string linea;
    cout<<mensaje;
    if (!getline(cin,linea)){
        guardarEstado();
        cout<<endl<<"se cerro la entrada de datos, el programa termina"<<endl;
        exit(0);
    }
    return linea;
}

// funcion recortarEspacios: quita los espacios y tabulaciones del inicio y del final de un texto
string recortarEspacios(string texto){
    int inicio=0;
    int fin=texto.length()-1;
    while (inicio<=fin && (texto[inicio]==' ' || texto[inicio]=='\t')){
        inicio++;
    }
    while (fin>=inicio && (texto[fin]==' ' || texto[fin]=='\t' || texto[fin]=='\r')){
        fin--;
    }
    return texto.substr(inicio,fin-inicio+1);
}

// funcion aMinusculas: convierte todas las letras de un texto a minusculas
string aMinusculas(string texto){
    int posicion;
    for (posicion=0;posicion<(int)texto.length();posicion++){
        texto[posicion]=tolower(texto[posicion]);
    }
    return texto;
}

// funcion textoALargo: convierte un texto de digitos (con signo menos opcional) en numero.
// si el texto no es un numero valido devuelve 0 (se usa al leer los archivos)
long long textoALargo(string texto){
    long long numero=0;
    int posicion=0;
    int negativo=0;
    texto=recortarEspacios(texto);
    if (texto.length()>0 && texto[0]=='-'){
        negativo=1;
        posicion=1;
    }
    for (;posicion<(int)texto.length();posicion++){
        if (texto[posicion]<'0' || texto[posicion]>'9'){
            return 0;
        }
        numero=numero*10+(texto[posicion]-'0');
    }
    if (negativo==1){
        numero=-numero;
    }
    return numero;
}

// funcion textoAEntero: igual que textoALargo pero devuelve un int
int textoAEntero(string texto){
    return (int)textoALargo(texto);
}

// funcion leerEntero: pide un numero entero positivo y repite la pregunta hasta que sea valido.
// valida que no este vacio, que sean solo digitos (sin letras, espacios ni simbolos),
// que no sea demasiado largo y que este entre minimo y maximo
int leerEntero(string mensaje, int minimo, int maximo){
    string texto;
    int numero, posicion, entradaValida;
    do{
        entradaValida=1;
        numero=0;
        texto=recortarEspacios(leerLinea(mensaje));
        if (texto.length()==0){
            cout<<"no puede dejar este dato vacio, escriba un numero"<<endl;
            entradaValida=0;
        } else if (texto.length()>9){ // mas de 9 digitos no cabe en un int
            cout<<"el numero es demasiado largo"<<endl;
            entradaValida=0;
        } else {
            for (posicion=0;posicion<(int)texto.length();posicion++){
                if (texto[posicion]>='0' && texto[posicion]<='9'){
                    numero=numero*10+(texto[posicion]-'0'); // armamos el numero digito por digito
                } else {
                    entradaValida=0;
                }
            }
            if (entradaValida==0){
                cout<<"entrada no valida, escriba solo numeros enteros (sin letras, espacios ni simbolos)"<<endl;
            }
        }
        if (entradaValida==1 && (numero<minimo || numero>maximo)){
            cout<<"el numero debe estar entre "<<minimo<<" y "<<maximo<<endl;
            entradaValida=0;
        }
    }while (entradaValida==0);
    return numero;
}

// funcion leerMontoCentavos: pide un monto en quetzales (por ejemplo 12 o 12.50) y lo devuelve en centavos.
// valida el formato (maximo 2 decimales), el rango y que sea multiplo de multiploCentavos
// (los montos son multiplos de 25 centavos porque no hay monedas mas pequenas)
int leerMontoCentavos(string mensaje, int minimoCentavos, int maximoCentavos, int multiploCentavos){
    string texto;
    int posicion, entradaValida, cantidadPuntos, cifrasDecimales, centavos, digito;
    long long quetzales, montoGrande;
    int montoCentavos=0;
    do{
        entradaValida=1;
        quetzales=0;
        centavos=0;
        cantidadPuntos=0;
        cifrasDecimales=0;
        texto=recortarEspacios(leerLinea(mensaje));
        if (texto.length()==0 || texto.length()>9){
            cout<<"escriba un monto valido, por ejemplo 25 o 12.50"<<endl;
            entradaValida=0;
        } else {
            for (posicion=0;posicion<(int)texto.length();posicion++){
                if (texto[posicion]=='.'){
                    cantidadPuntos++;
                } else if (texto[posicion]>='0' && texto[posicion]<='9'){
                    digito=texto[posicion]-'0';
                    if (cantidadPuntos==0){
                        quetzales=quetzales*10+digito;
                    } else {
                        cifrasDecimales++;
                        if (cifrasDecimales==1){
                            centavos=digito*10; // primer decimal: decenas de centavo
                        } else if (cifrasDecimales==2){
                            centavos=centavos+digito;
                        }
                    }
                } else {
                    entradaValida=0;
                }
            }
            if (entradaValida==0 || cantidadPuntos>1 || cifrasDecimales>2 || texto[0]=='.' || (cantidadPuntos==1 && cifrasDecimales==0)){
                cout<<"monto no valido, escriba solo numeros con maximo 2 decimales, por ejemplo 25 o 12.50"<<endl;
                entradaValida=0;
            }
        }
        if (entradaValida==1){
            montoGrande=quetzales*100+centavos;
            if (montoGrande<minimoCentavos || montoGrande>maximoCentavos){
                cout<<"el monto debe estar entre "<<dinero(minimoCentavos)<<" y "<<dinero(maximoCentavos)<<endl;
                entradaValida=0;
            } else if (montoGrande%multiploCentavos!=0){
                cout<<"el monto debe ser multiplo de "<<dinero(multiploCentavos)<<endl;
                entradaValida=0;
            } else {
                montoCentavos=(int)montoGrande;
            }
        }
    }while (entradaValida==0);
    return montoCentavos;
}

// funcion leerTextoSinEspacios: pide una palabra (sin espacios) con un largo entre minimo y maximo.
// se usa para usuarios y NIT
string leerTextoSinEspacios(string mensaje, int largoMinimo, int largoMaximo){
    string texto;
    int entradaValida, posicion;
    do{
        entradaValida=1;
        texto=recortarEspacios(leerLinea(mensaje));
        if (texto.length()==0){
            cout<<"no puede dejar este dato vacio"<<endl;
            entradaValida=0;
        } else {
            for (posicion=0;posicion<(int)texto.length();posicion++){
                if (texto[posicion]==' ' || texto[posicion]=='\t'){
                    entradaValida=0;
                }
            }
            if (entradaValida==0){
                cout<<"este dato no puede llevar espacios"<<endl;
            } else if ((int)texto.length()<largoMinimo || (int)texto.length()>largoMaximo){
                cout<<"debe tener entre "<<largoMinimo<<" y "<<largoMaximo<<" caracteres"<<endl;
                entradaValida=0;
            }
        }
    }while (entradaValida==0);
    return texto;
}

// funcion leerTextoConEspacios: pide un texto (puede llevar espacios) con un largo entre minimo y maximo.
// se usa para nombres y descripciones
string leerTextoConEspacios(string mensaje, int largoMinimo, int largoMaximo){
    string texto;
    int entradaValida;
    do{
        entradaValida=1;
        texto=recortarEspacios(leerLinea(mensaje));
        if (texto.length()==0){
            cout<<"no puede dejar este dato vacio"<<endl;
            entradaValida=0;
        } else if ((int)texto.length()<largoMinimo || (int)texto.length()>largoMaximo){
            cout<<"debe tener entre "<<largoMinimo<<" y "<<largoMaximo<<" caracteres"<<endl;
            entradaValida=0;
        }
    }while (entradaValida==0);
    return texto;
}

// funcion leerContrasena: pide la contrasena. En windows muestra un asterisco por cada tecla
// (usando _getch); en otros sistemas, o si OCULTAR_CONTRASENA vale 0, la muestra normal
string leerContrasena(string mensaje){
#if defined(_WIN32) && OCULTAR_CONTRASENA==1
    string contrasena="";
    int tecla;
    cout<<mensaje;
    do{
        tecla=_getch(); // lee una tecla sin mostrarla
        if (tecla==0 || tecla==224){
            _getch(); // teclas especiales (flechas): se ignora la segunda parte
        } else if (tecla==8){ // retroceso: borra el ultimo caracter
            if (contrasena.length()>0){
                contrasena.erase(contrasena.length()-1);
                cout<<"\b \b";
            }
        } else if (tecla>=32 && tecla<=126){ // caracter normal: se guarda y se muestra un asterisco
            if (contrasena.length()<30){
                contrasena+=(char)tecla;
                cout<<"*";
            }
        }
    }while (tecla!='\r' && tecla!='\n');
    cout<<endl;
    return contrasena;
#else
    return leerLinea(mensaje);
#endif
}

// funcion esNitValido: devuelve 1 si el NIT es CF o son solo numeros (que pueden terminar en K)
int esNitValido(string nit){
    int posicion, cantidadDigitos=0;
    nit=aMinusculas(nit);
    if (nit=="cf"){
        return 1;
    }
    for (posicion=0;posicion<(int)nit.length();posicion++){
        if (nit[posicion]>='0' && nit[posicion]<='9'){
            cantidadDigitos++;
        } else if (!(nit[posicion]=='k' && posicion==(int)nit.length()-1)){ // la K solo puede ser el ultimo caracter
            return 0;
        }
    }
    if (cantidadDigitos==0){
        return 0;
    }
    return 1;
}

// funcion dinero: convierte centavos en texto con formato de quetzales, por ejemplo 7250 -> Q72.50
string dinero(int centavos){
    string signo="";
    string centavosTexto;
    if (centavos<0){
        signo="-";
        centavos=-centavos;
    }
    centavosTexto=to_string(centavos%100);
    if (centavos%100<10){ // si los centavos son menores a 10 se agrega un 0 antes
        centavosTexto="0"+centavosTexto;
    }
    return signo+"Q"+to_string(centavos/100)+"."+centavosTexto;
}

// funcion numeroConCeros: convierte un numero en texto con ceros a la izquierda, por ejemplo 7 con 4 digitos -> 0007
string numeroConCeros(int numero, int cantidadDigitos){
    string texto=to_string(numero);
    while ((int)texto.length()<cantidadDigitos){
        texto="0"+texto;
    }
    return texto;
}

// funcion formatearFechaHora: convierte un momento en texto usando un formato de strftime,
// por ejemplo "%Y-%m-%d %H:%M:%S" da 2026-10-23 19:30:05
string formatearFechaHora(time_t momento, string formato){
    char texto[80]; // 80 = espacio de sobra para una fecha con hora
    struct tm *tiempoLocal=localtime(&momento);
    strftime(texto,sizeof(texto),formato.c_str(),tiempoLocal);
    return string(texto);
}
// funcion cifrarMurcielago: cambia cada letra de la palabra MURCIELAGO por su numero
// (M=0 U=1 R=2 C=3 I=4 E=5 L=6 A=7 G=8 O=9, en mayuscula o minuscula).
// las demas letras, numeros y simbolos no cambian. Para validar una contrasena no hace
// falta descifrar: se cifra lo que escribe el usuario y se compara con la contrasena guardada
string cifrarMurcielago(string texto){
    string resultado="";
    int posicion;
    for (posicion=0;posicion<(int)texto.length();posicion++){
        switch (tolower(texto[posicion])){
            case 'm': resultado+='0'; break;
            case 'u': resultado+='1'; break;
            case 'r': resultado+='2'; break;
            case 'c': resultado+='3'; break;
            case 'i': resultado+='4'; break;
            case 'e': resultado+='5'; break;
            case 'l': resultado+='6'; break;
            case 'a': resultado+='7'; break;
            case 'g': resultado+='8'; break;
            case 'o': resultado+='9'; break;
            default: resultado+=texto[posicion]; break; // las demas letras quedan igual
        }
    }
    return resultado;
}

// funcion buscarUsuario: devuelve la posicion del usuario en la lista (sin importar mayusculas)
// o -1 si no existe
int buscarUsuario(string usuarioBuscado){
    int posicion;
    for (posicion=0;posicion<cantidadUsuarios;posicion++){
        if (aMinusculas(nombreUsuario[posicion])==aMinusculas(usuarioBuscado)){
            return posicion;
        }
    }
    return -1;
}

// funcion iniciarSesion: pantalla de login. Pide usuario y contrasena (con validaciones de
// formato), cuenta los intentos fallidos y despues de maximoIntentosLogin bloquea el acceso
// unos segundos. Escribir 0 como usuario cierra el programa. No termina hasta iniciar sesion
void iniciarSesion(){
    string usuarioIngresado, contrasenaIngresada;
    int posicionUsuario, intentosFallidos=0, segundosRestantes, sesionIniciada=0, contrasenaValida;
    while (sesionIniciada==0){
        system("cls");
        cout<<"================ INICIO DE SESION ================"<<endl;
        cout<<"(escriba 0 como usuario para salir del programa)"<<endl<<endl;
        usuarioIngresado=leerTextoSinEspacios("usuario: ",1,20);
        if (usuarioIngresado=="0"){
            cout<<"hasta pronto"<<endl;
            exit(0);
        }
        do{ // la contrasena no puede ir vacia, ni llevar espacios, y debe tener entre 4 y 30 caracteres
            contrasenaValida=1;
            contrasenaIngresada=leerContrasena("contrasena: ");
            if (contrasenaIngresada.length()<4 || contrasenaIngresada.length()>30){
                cout<<"la contrasena debe tener entre 4 y 30 caracteres"<<endl;
                contrasenaValida=0;
            } else if (contrasenaIngresada.find(' ')!=string::npos){
                cout<<"la contrasena no puede llevar espacios"<<endl;
                contrasenaValida=0;
            }
        }while (contrasenaValida==0);
        posicionUsuario=buscarUsuario(usuarioIngresado);
        // se compara la contrasena cifrada con la guardada; el mensaje no dice cual de los dos datos fallo
        if (posicionUsuario!=-1 && cifrarMurcielago(contrasenaIngresada)==contrasenaCifradaUsuario[posicionUsuario]){
            sesionIniciada=1;
            usuarioActual=posicionUsuario;
            rolActual=rolUsuario[posicionUsuario];
            sesionAbierta=1;
            cout<<endl<<"bienvenido "<<nombreCompletoUsuario[usuarioActual]<<" ("<<nombreRol[rolActual]<<")"<<endl;
            system("pause");
        } else {
            intentosFallidos++;
            cout<<"usuario o contrasena incorrectos (intento "<<intentosFallidos<<" de "<<maximoIntentosLogin<<")"<<endl;
            if (intentosFallidos>=maximoIntentosLogin){ // bloqueo temporal con cuenta regresiva
                for (segundosRestantes=segundosBloqueoLogin;segundosRestantes>0;segundosRestantes--){
                    cout<<"\racceso bloqueado por demasiados intentos, espere "<<segundosRestantes<<" segundos...   "<<flush;
                    this_thread::sleep_for(chrono::seconds(1));
                }
                cout<<endl;
                intentosFallidos=0;
            }
            system("pause");
        }
    }
}

// funcion autorizarComoAdministrador: si quien usa el programa es administrador devuelve 1 de inmediato.
// si no, pide el usuario y la contrasena de un administrador (con maximoIntentosLogin intentos)
// y devuelve 1 si son correctos o 0 si se cancela o se equivoca demasiadas veces
int autorizarComoAdministrador(string motivo){
    string usuarioAdministrador, contrasenaAdministrador;
    int posicion, intentos=0;
    if (rolActual==1){
        return 1;
    }
    cout<<"se necesita la autorizacion de un administrador para: "<<motivo<<endl;
    while (intentos<maximoIntentosLogin){
        usuarioAdministrador=leerTextoSinEspacios("usuario del administrador (0 para cancelar): ",1,20);
        if (usuarioAdministrador=="0"){
            cout<<"autorizacion cancelada"<<endl;
            return 0;
        }
        contrasenaAdministrador=leerContrasena("contrasena del administrador: ");
        posicion=buscarUsuario(usuarioAdministrador);
        if (posicion!=-1 && rolUsuario[posicion]==1 && cifrarMurcielago(contrasenaAdministrador)==contrasenaCifradaUsuario[posicion]){
            cout<<"autorizado"<<endl;
            return 1;
        }
        intentos++;
        cout<<"datos incorrectos o el usuario no es administrador (intento "<<intentos<<" de "<<maximoIntentosLogin<<")"<<endl;
    }
    cout<<"autorizacion denegada"<<endl;
    return 0;
}

// funcion tienePermiso: devuelve 1 si el rol del usuario actual es igual o menor al rol maximo permitido
// (1 administrador, 2 cajero, 3 mesero). si no tiene permiso muestra un aviso y devuelve 0
int tienePermiso(int rolMaximoPermitido){
    if (rolActual<=rolMaximoPermitido){
        return 1;
    }
    cout<<"su rol ("<<nombreRol[rolActual]<<") no tiene permiso para esta opcion"<<endl;
    return 0;
}
// funcion mostrarEncabezado: muestra en la parte de arriba de cada pantalla el nombre del restaurante,
// la fecha y hora, quien tiene la sesion iniciada y si la caja esta abierta
void mostrarEncabezado(){
    cout<<"================================================================"<<endl;
    cout<<" RESTAURANTE - "<<formatearFechaHora(time(NULL),"%Y-%m-%d %H:%M")<<endl;
    cout<<" usuario: "<<nombreCompletoUsuario[usuarioActual]<<" ("<<nombreRol[rolActual]<<")";
    if (cajaAbierta==1){
        cout<<" | caja: ABIERTA"<<endl;
    } else {
        cout<<" | caja: CERRADA"<<endl;
    }
    cout<<"================================================================"<<endl;
}

// funcion mostrarOpcionesMenuPrincipal: muestra las opciones del menu principal y a que rol
// esta reservada cada una
void mostrarOpcionesMenuPrincipal(){
    cout<<endl;
    cout<<"1. Mesas y reservas"<<endl;
    cout<<"2. Tomar orden (mesa o para llevar)"<<endl;
    cout<<"3. Anular producto de una cuenta (requiere administrador)"<<endl;
    cout<<"4. Precuenta y cobro (cajero y administrador)"<<endl;
    cout<<"5. Cola de cocina y barra"<<endl;
    cout<<"6. Caja: apertura, retiros, gastos y estado (cajero y administrador)"<<endl;
    cout<<"7. Administrar menu de productos (solo administrador)"<<endl;
    cout<<"8. Finalizar dia y cuadre de caja (cajero y administrador)"<<endl;
    cout<<"0. Cerrar sesion"<<endl;
}

// funcion menuMesas: submenu con todo lo relacionado a las mesas. Se repite hasta elegir 0
void menuMesas(){
    int opcion;
    do{
        system("cls");
        mostrarEstadoCuentas();
        cout<<endl;
        cout<<"---------------------- mesas y reservas ------------------------"<<endl;
        cout<<"1. Reservar mesa"<<endl;
        cout<<"2. Cancelar reserva"<<endl;
        cout<<"3. Sentar clientes"<<endl;
        cout<<"4. Mover la cuenta a otra mesa"<<endl;
        cout<<"5. Liberar mesa sin consumo"<<endl;
        cout<<"0. Volver al menu principal"<<endl;
        opcion=leerEntero("seleccione una opcion: ",0,5);
        switch (opcion){
            case 1:
                reservarMesa();
                break;
            case 2:
                cancelarReserva();
                break;
            case 3:
                sentarClientes();
                break;
            case 4:
                moverCuenta();
                break;
            case 5:
                liberarMesaSinConsumo();
                break;
        }
        if (opcion!=0){
            system("pause");
        }
    }while (opcion!=0);
}

// funcion menuCaja: submenu de la caja (abrir, registrar retiros o gastos y ver el estado)
void menuCaja(){
    int opcion;
    do{
        system("cls");
        cout<<"---------------------- caja ------------------------"<<endl;
        cout<<"1. Abrir caja"<<endl;
        cout<<"2. Registrar retiro o gasto"<<endl;
        cout<<"3. Ver estado de la caja"<<endl;
        cout<<"0. Volver al menu principal"<<endl;
        opcion=leerEntero("seleccione una opcion: ",0,3);
        switch (opcion){
            case 1:
                aperturaCaja();
                break;
            case 2:
                registrarMovimientoCaja();
                break;
            case 3:
                mostrarEstadoCaja();
                break;
        }
        if (opcion!=0){
            system("pause");
        }
    }while (opcion!=0);
}

// funcion administrarMenu: el administrador puede agregar productos, cambiar precios,
// marcar productos como agotados o desactivarlos. Cada cambio se guarda en menu.txt
void administrarMenu(){
    int opcion, producto, categoria, precioCentavos, posicion, nombreRepetido;
    string nombre;
    do{
        system("cls");
        mostrarMenuProductos(1);
        cout<<endl;
        cout<<"---------------------- administrar menu ------------------------"<<endl;
        cout<<"1. Agregar producto"<<endl;
        cout<<"2. Cambiar precio de un producto"<<endl;
        cout<<"3. Marcar producto como agotado o disponible"<<endl;
        cout<<"4. Desactivar o activar producto"<<endl;
        cout<<"0. Volver al menu principal"<<endl;
        opcion=leerEntero("seleccione una opcion: ",0,4);
        if (opcion==1){
            if (cantidadProductosMenu>=40){ // 40 = maximo de productos que caben en los arreglos del menu
                cout<<"el menu ya tiene el maximo de productos (40)"<<endl;
            } else {
                nombre=leerTextoConEspacios("nombre del producto: ",2,30);
                nombreRepetido=0;
                for (posicion=0;posicion<cantidadProductosMenu;posicion++){
                    if (aMinusculas(nombreProducto[posicion])==aMinusculas(nombre)){
                        nombreRepetido=1;
                    }
                }
                if (nombreRepetido==1){
                    cout<<"ya existe un producto con ese nombre"<<endl;
                } else {
                    categoria=leerEntero("categoria: 1. Bebidas / 2. Pizzas / 3. Otros: ",1,3);
                    precioCentavos=leerMontoCentavos("precio en quetzales (multiplo de 0.25): ",25,1000000,25);
                    agregarProductoAlMenu(nombre,precioCentavos,categoria);
                    guardarMenu();
                    cout<<"producto agregado al menu"<<endl;
                }
            }
        } else if (opcion>=2 && opcion<=4){
            producto=leerEntero("numero del producto (0 para cancelar): ",0,cantidadProductosMenu);
            if (producto>0){
                producto=producto-1;
                if (opcion==2){
                    if (productoEnUsoEnCuentas(producto)==1){ // cambiar el precio de algo que ya esta en una cuenta abierta alteraria esa cuenta
                        cout<<"ese producto esta en una cuenta abierta, cambie el precio cuando no haya cuentas abiertas con el"<<endl;
                    } else {
                        precioCentavos=leerMontoCentavos("nuevo precio en quetzales (multiplo de 0.25): ",25,1000000,25);
                        precioProductoCentavos[producto]=precioCentavos;
                        guardarMenu();
                        cout<<"precio actualizado"<<endl;
                    }
                } else if (opcion==3){
                    productoAgotado[producto]=1-productoAgotado[producto]; // si valia 1 pasa a 0 y al reves
                    guardarMenu();
                    if (productoAgotado[producto]==1){
                        cout<<nombreProducto[producto]<<" marcado como AGOTADO"<<endl;
                    } else {
                        cout<<nombreProducto[producto]<<" vuelve a estar disponible"<<endl;
                    }
                } else {
                    productoActivo[producto]=1-productoActivo[producto];
                    guardarMenu();
                    if (productoActivo[producto]==1){
                        cout<<nombreProducto[producto]<<" activado en el menu"<<endl;
                    } else {
                        cout<<nombreProducto[producto]<<" desactivado (ya no aparece en el menu)"<<endl;
                    }
                }
            }
        }
        if (opcion!=0){
            system("pause");
        }
    }while (opcion!=0);
}
// funcion agregarProductoAlMenu: agrega un producto nuevo al final de la lista del menu
// (activo y disponible). El precio va en centavos y la categoria es 1 bebidas, 2 pizzas o 3 otros
void agregarProductoAlMenu(string nombre, int precioCentavos, int categoria){
    nombreProducto[cantidadProductosMenu]=nombre;
    precioProductoCentavos[cantidadProductosMenu]=precioCentavos;
    categoriaProducto[cantidadProductosMenu]=categoria;
    productoAgotado[cantidadProductosMenu]=0;
    productoActivo[cantidadProductosMenu]=1;
    cantidadProductosMenu++;
}

// funcion crearMenuPorDefecto: carga los 6 productos iniciales (se usa cuando todavia no existe menu.txt)
void crearMenuPorDefecto(){
    cantidadProductosMenu=0;
    agregarProductoAlMenu("cocacola",600,1);
    agregarProductoAlMenu("pepsi",600,1);
    agregarProductoAlMenu("seven-up",500,1);
    agregarProductoAlMenu("pizza peperoni mediana",6000,2);
    agregarProductoAlMenu("pizza jamon mediana",5500,2);
    agregarProductoAlMenu("pizza hawaiana",6500,2);
}

// funcion mostrarMenuProductos: muestra el menu completo de una sola vez, agrupado por categoria,
// con el numero de cada producto. Si mostrarOcultos vale 1 tambien muestra los desactivados
// (se usa en la pantalla del administrador). Los agotados se marcan
void mostrarMenuProductos(int mostrarOcultos){
    int categoria, producto, hayProductosEnCategoria;
    cout<<"---------------------- menu del restaurante ------------------------"<<endl;
    for (categoria=1;categoria<=cantidadCategorias;categoria++){
        hayProductosEnCategoria=0;
        for (producto=0;producto<cantidadProductosMenu;producto++){
            if (categoriaProducto[producto]==categoria && (productoActivo[producto]==1 || mostrarOcultos==1)){
                if (hayProductosEnCategoria==0){
                    cout<<nombreCategoria[categoria-1]<<":"<<endl;
                    hayProductosEnCategoria=1;
                }
                cout<<"  "<<producto+1<<". "<<nombreProducto[producto]<<" - "<<dinero(precioProductoCentavos[producto]);
                if (productoAgotado[producto]==1){
                    cout<<"  (AGOTADO)";
                }
                if (productoActivo[producto]==0){
                    cout<<"  (desactivado)";
                }
                cout<<endl;
            }
        }
    }
}

// funcion productoEnUsoEnCuentas: devuelve 1 si algun producto esta en alguna cuenta abierta
int productoEnUsoEnCuentas(int producto){
    int cuenta;
    for (cuenta=0;cuenta<cantidadCuentas;cuenta++){
        if (productosEnCuenta[cuenta][producto]>0){
            return 1;
        }
    }
    return 0;
}
// funcion etiquetaCuenta: devuelve el nombre de una cuenta para mostrarlo, por ejemplo
// "Mesa 3" o "Para llevar 2 (Carlos)"
string etiquetaCuenta(int cuenta){
    string texto;
    if (cuenta<cantidadMesas){
        return "Mesa "+to_string(cuenta+1);
    }
    texto="Para llevar "+to_string(cuenta-cantidadMesas+1);
    if (nombreCuenta[cuenta]!=""){
        texto=texto+" ("+nombreCuenta[cuenta]+")";
    }
    return texto;
}

// funcion totalCuentaCentavos: devuelve el total de una cuenta (suma cantidad por precio de cada producto)
int totalCuentaCentavos(int cuenta){
    int producto, total=0;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        total+=productosEnCuenta[cuenta][producto]*precioProductoCentavos[producto];
    }
    return total;
}

// funcion hayProductosEnCuenta: devuelve 1 si la cuenta tiene al menos un producto
int hayProductosEnCuenta(int cuenta){
    int producto;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        if (productosEnCuenta[cuenta][producto]>0){
            return 1;
        }
    }
    return 0;
}

// funcion mostrarCuenta: muestra los productos de una cuenta con su subtotal y el total
void mostrarCuenta(int cuenta){
    int producto;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        if (productosEnCuenta[cuenta][producto]>0){
            cout<<"\t"<<producto+1<<". "<<productosEnCuenta[cuenta][producto]<<" x "<<nombreProducto[producto]<<" ("<<dinero(precioProductoCentavos[producto])<<" c/u) = "<<dinero(productosEnCuenta[cuenta][producto]*precioProductoCentavos[producto])<<endl;
        }
    }
    cout<<"\ttotal de la cuenta: "<<dinero(totalCuentaCentavos(cuenta))<<endl;
}

// funcion mostrarEstadoCuentas: muestra la lista de las mesas con sus sillas y su estado
// (liberada, ocupada con su cuenta actual o reservada con el nombre) y las ordenes para llevar abiertas
void mostrarEstadoCuentas(){
    int cuenta, hayParaLlevar=0;
    cout<<"---------------------- estado de mesas ------------------------"<<endl;
    for (cuenta=0;cuenta<cantidadMesas;cuenta++){
        cout<<" "<<cuenta+1<<". Mesa "<<cuenta+1<<" | sillas: "<<sillasMesa[cuenta]<<" | ";
        if (estadoCuenta[cuenta]==0){
            cout<<"liberada";
        } else if (estadoCuenta[cuenta]==1){
            cout<<"OCUPADA | cuenta: "<<dinero(totalCuentaCentavos(cuenta));
        } else {
            cout<<"RESERVADA a nombre de "<<nombreCuenta[cuenta];
        }
        cout<<endl;
    }
    for (cuenta=cantidadMesas;cuenta<cantidadCuentas;cuenta++){
        if (estadoCuenta[cuenta]==1){
            if (hayParaLlevar==0){
                cout<<"---------------------- ordenes para llevar ------------------------"<<endl;
                hayParaLlevar=1;
            }
            cout<<" "<<cuenta+1<<". "<<etiquetaCuenta(cuenta)<<" | cuenta: "<<dinero(totalCuentaCentavos(cuenta))<<endl;
        }
    }
}

// funcion elegirCuenta: pide el numero de una cuenta y valida que sirva para lo que se quiere hacer.
// devuelve la posicion de la cuenta, o -1 si se escribe 0 para cancelar. Segun el filtro:
int elegirCuenta(string mensaje, int filtro){
    int numero, cuenta, cuentaValida;
    do{
        cuentaValida=1;
        numero=leerEntero(mensaje+" (0 para cancelar): ",0,cantidadCuentas);
        if (numero==0){
            return -1;
        }
        cuenta=numero-1; // las cuentas empiezan en 0
        if (filtro==1 && estadoCuenta[cuenta]!=1){
            cout<<"esa cuenta no esta abierta"<<endl;
            cuentaValida=0;
        } else if (filtro==2 && !(cuenta<cantidadMesas && estadoCuenta[cuenta]==2)){
            cout<<"esa no es una mesa reservada"<<endl;
            cuentaValida=0;
        } else if (filtro==3 && !(cuenta<cantidadMesas && estadoCuenta[cuenta]==0)){
            cout<<"esa no es una mesa liberada"<<endl;
            cuentaValida=0;
        } else if (filtro==5 && !(cuenta<cantidadMesas && estadoCuenta[cuenta]!=1)){
            cout<<"esa mesa esta ocupada o no es una mesa"<<endl;
            cuentaValida=0;
        } else if (filtro==6 && !(cuenta<cantidadMesas && estadoCuenta[cuenta]==1)){
            cout<<"esa no es una mesa ocupada"<<endl;
            cuentaValida=0;
        }
    }while (cuentaValida==0);
    return cuenta;
}

// funcion reservarMesa: pregunta para cuantas personas es la mesa, muestra solo las mesas liberadas
// con sillas suficientes, deja elegir una y la pasa a reservada con el nombre de quien reserva
void reservarMesa(){
    int personas, mesa, disponibles=0, mesaValida;
    cout<<"---------------------- reservar mesa ------------------------"<<endl;
    personas=leerEntero("para cuantas personas necesita mesa: ",1,50);
    cout<<"mesas disponibles para "<<personas<<" personas:"<<endl;
    for (mesa=0;mesa<cantidadMesas;mesa++){
        if (estadoCuenta[mesa]==0 && sillasMesa[mesa]>=personas){ // liberada y con sillas suficientes
            cout<<"\tMesa "<<mesa+1<<" ("<<sillasMesa[mesa]<<" sillas)"<<endl;
            disponibles++;
        }
    }
    if (disponibles==0){
        cout<<"no hay mesas disponibles para esa cantidad de personas"<<endl;
        return;
    }
    do{ // validamos que la mesa elegida este en la lista de disponibles
        mesaValida=1;
        mesa=leerEntero("numero de la mesa que desea reservar (0 para cancelar): ",0,cantidadMesas);
        if (mesa!=0 && (estadoCuenta[mesa-1]!=0 || sillasMesa[mesa-1]<personas)){
            cout<<"la mesa elegida no esta en la lista de mesas disponibles"<<endl;
            mesaValida=0;
        }
    }while (mesaValida==0);
    if (mesa==0){
        return;
    }
    nombreCuenta[mesa-1]=leerTextoConEspacios("nombre de quien reserva: ",2,30);
    estadoCuenta[mesa-1]=2; // la mesa queda reservada
    cout<<"la mesa "<<mesa<<" fue reservada a nombre de "<<nombreCuenta[mesa-1]<<endl;
}

// funcion cancelarReserva: elige una mesa reservada, pide confirmacion y la deja liberada
void cancelarReserva(){
    int mesa, opcion;
    cout<<"---------------------- cancelar reserva ------------------------"<<endl;
    mesa=elegirCuenta("numero de la mesa reservada que se cancela",2);
    if (mesa==-1){
        return;
    }
    opcion=leerEntero("cancelar la reserva a nombre de "+nombreCuenta[mesa]+"? 1. Si / 2. No: ",1,2);
    if (opcion==1){
        estadoCuenta[mesa]=0;
        nombreCuenta[mesa]="";
        cout<<"reserva cancelada, la mesa "<<mesa+1<<" quedo liberada"<<endl;
    }
}

// funcion sentarClientes: sienta a los clientes en una mesa liberada o reservada (cuando llegan a su reserva).
// pregunta cuantas personas son y valida que quepan; la mesa pasa a ocupada
void sentarClientes(){
    int mesa, personas;
    cout<<"---------------------- sentar clientes ------------------------"<<endl;
    mesa=elegirCuenta("numero de la mesa donde se sientan los clientes",5);
    if (mesa==-1){
        return;
    }
    personas=leerEntero("cuantas personas son: ",1,50);
    if (personas>sillasMesa[mesa]){
        cout<<"la mesa "<<mesa+1<<" solo tiene "<<sillasMesa[mesa]<<" sillas, elija otra mesa"<<endl;
        return;
    }
    estadoCuenta[mesa]=1; // si estaba reservada se conserva el nombre de la reserva
    cout<<"clientes sentados en la mesa "<<mesa+1<<endl;
}

// funcion moverCuenta: pasa la cuenta de una mesa ocupada a otra mesa liberada (por ejemplo si los
// clientes cambian de lugar). Tambien cambia de mesa los pedidos pendientes de la cola
void moverCuenta(){
    int origen, destino, producto, pedido;
    cout<<"---------------------- mover cuenta ------------------------"<<endl;
    origen=elegirCuenta("numero de la mesa de origen",6);
    if (origen==-1){
        return;
    }
    destino=elegirCuenta("numero de la mesa de destino (debe estar liberada)",3);
    if (destino==-1){
        return;
    }
    for (producto=0;producto<cantidadProductosMenu;producto++){
        productosEnCuenta[destino][producto]=productosEnCuenta[origen][producto];
        productosEnCuenta[origen][producto]=0;
    }
    nombreCuenta[destino]=nombreCuenta[origen];
    estadoCuenta[destino]=1;
    estadoCuenta[origen]=0;
    nombreCuenta[origen]="";
    for (pedido=0;pedido<totalEntradasCola;pedido++){ // los pedidos pendientes ahora se llevan a la mesa nueva
        if (colaCuenta[pedido]==origen && colaEntregada[pedido]==0){
            colaCuenta[pedido]=destino;
        }
    }
    cout<<"la cuenta de la mesa "<<origen+1<<" se paso a la mesa "<<destino+1<<endl;
}

// funcion liberarMesaSinConsumo: libera una mesa ocupada que no pidio nada (por ejemplo si los clientes se fueron).
// si la mesa ya tiene productos no se puede liberar, hay que cobrarla
void liberarMesaSinConsumo(){
    int mesa;
    cout<<"---------------------- liberar mesa sin consumo ------------------------"<<endl;
    mesa=elegirCuenta("numero de la mesa ocupada que se libera",6);
    if (mesa==-1){
        return;
    }
    if (hayProductosEnCuenta(mesa)==1){
        cout<<"la mesa tiene productos en su cuenta, debe cobrarse"<<endl;
        return;
    }
    estadoCuenta[mesa]=0;
    nombreCuenta[mesa]="";
    cout<<"la mesa "<<mesa+1<<" quedo liberada"<<endl;
}
// funcion mostrarPedidoNuevo: muestra lo que lleva el pedido que se esta armando (todavia no enviado)
// con su total, o avisa si esta vacio
void mostrarPedidoNuevo(int pedido[]){
    int producto, totalUnidades=0, totalCentavos=0;
    cout<<endl;
    cout<<"---------------------- pedido actual (aun no enviado) ------------------------"<<endl;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        if (pedido[producto]>0){
            cout<<"\t"<<pedido[producto]<<" x "<<nombreProducto[producto]<<" = "<<dinero(pedido[producto]*precioProductoCentavos[producto])<<endl;
            totalUnidades+=pedido[producto];
            totalCentavos+=pedido[producto]*precioProductoCentavos[producto];
        }
    }
    if (totalUnidades==0){
        cout<<"\t(vacio)"<<endl;
    } else {
        cout<<"\ttotal del pedido: "<<dinero(totalCentavos)<<endl;
    }
}

// funcion enviarPedido: confirma un pedido. Suma sus productos a la cuenta, deja la cuenta ocupada
// y crea los pedidos de la cola: uno para la barra con las bebidas y otro para la cocina con
// el resto de productos (solo se crea el que tenga productos)
void enviarPedido(int cuenta, int pedido[]){
    int producto, hayBebidas=0, hayComida=0, destino, esDelDestino;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        if (pedido[producto]>0){
            productosEnCuenta[cuenta][producto]+=pedido[producto]; // se suma a la cuenta (una sola cuenta por mesa)
            if (categoriaProducto[producto]==1){
                hayBebidas=1;
            } else {
                hayComida=1;
            }
        }
    }
    estadoCuenta[cuenta]=1; // la cuenta queda ocupada (si era una reserva se conserva el nombre)
    for (destino=1;destino<=2;destino++){ // 1 cocina, 2 barra
        if ((destino==1 && hayComida==1) || (destino==2 && hayBebidas==1)){
            colaCuenta[totalEntradasCola]=cuenta;
            colaDestino[totalEntradasCola]=destino;
            colaHoraTomada[totalEntradasCola]=time(NULL);
            colaEntregada[totalEntradasCola]=0;
            for (producto=0;producto<cantidadProductosMenu;producto++){
                colaCantidad[totalEntradasCola][producto]=0;
                esDelDestino=0;
                if (destino==2 && categoriaProducto[producto]==1){
                    esDelDestino=1;
                }
                if (destino==1 && categoriaProducto[producto]!=1){
                    esDelDestino=1;
                }
                if (esDelDestino==1 && pedido[producto]>0){
                    colaCantidad[totalEntradasCola][producto]=pedido[producto];
                }
            }
            totalEntradasCola++;
        }
    }
    cout<<endl;
    cout<<"pedido enviado a cocina y barra, cuenta actual de "<<etiquetaCuenta(cuenta)<<":"<<endl;
    mostrarCuenta(cuenta);
}

// funcion tomarOrden: elige una mesa o una orden para llevar y arma el pedido con el menu completo a la vista.
// El mesero escribe el numero del producto y la cantidad, y repite solo con lo que el cliente pide.
// Con 0 puede enviar el pedido, quitar un producto o cancelarlo. No deja agregar productos agotados
void tomarOrden(){
    int cuentaElegida, productoElegido, producto, cantidadElegida, opcionSubmenu, terminado, totalUnidades, cantidadQuitar;
    int pedidoNuevo[40]; // 40 = maximo de productos (unidades de cada producto del pedido que se esta armando)
    cout<<"---------------------- tomar orden ------------------------"<<endl;
    mostrarEstadoCuentas();
    cout<<endl;
    cout<<"para una orden nueva para llevar elija un numero de "<<cantidadMesas+1<<" a "<<cantidadCuentas<<" que no este en uso"<<endl;
    cuentaElegida=elegirCuenta("numero de la mesa o de la orden para llevar",4);
    if (cuentaElegida==-1){
        return;
    }
    if (totalEntradasCola+2>capacidadCola){ // cada pedido puede crear hasta 2 entradas en la cola (cocina y barra)
        cout<<"la cola de cocina y barra esta llena, no se pueden tomar mas ordenes hoy"<<endl;
        return;
    }
    if (cuentaElegida>=cantidadMesas && estadoCuenta[cuentaElegida]==0){ // orden para llevar nueva: se pide el nombre del cliente
        nombreCuenta[cuentaElegida]=leerTextoConEspacios("nombre del cliente para llevar: ",1,30);
    }
    for (producto=0;producto<cantidadProductosMenu;producto++){
        pedidoNuevo[producto]=0;
    }
    terminado=0;
    do{
        system("cls");
        cout<<"TOMAR ORDEN - "<<etiquetaCuenta(cuentaElegida)<<endl;
        mostrarMenuProductos(0);
        mostrarPedidoNuevo(pedidoNuevo);
        cout<<endl;
        productoElegido=leerEntero("numero de producto a agregar (0 = enviar, quitar o cancelar el pedido): ",0,cantidadProductosMenu);
        if (productoElegido>0){
            producto=productoElegido-1;
            if (productoActivo[producto]==0){
                cout<<"ese producto no esta en el menu"<<endl;
                system("pause");
            } else if (productoAgotado[producto]==1){
                cout<<nombreProducto[producto]<<" esta agotado"<<endl;
                system("pause");
            } else {
                cantidadElegida=leerEntero("cantidad de "+nombreProducto[producto]+" (0 para no agregar): ",0,99-pedidoNuevo[producto]);
                pedidoNuevo[producto]+=cantidadElegida;
            }
        } else {
            opcionSubmenu=leerEntero("1. Enviar pedido / 2. Quitar un producto del pedido / 3. Cancelar el pedido / 4. Seguir agregando: ",1,4);
            if (opcionSubmenu==1){
                totalUnidades=0;
                for (producto=0;producto<cantidadProductosMenu;producto++){
                    totalUnidades+=pedidoNuevo[producto];
                }
                if (totalUnidades==0){
                    cout<<"el pedido esta vacio, agregue al menos un producto"<<endl;
                    system("pause");
                } else {
                    enviarPedido(cuentaElegida,pedidoNuevo);
                    terminado=1;
                }
            } else if (opcionSubmenu==2){
                productoElegido=leerEntero("numero del producto a quitar (0 para volver): ",0,cantidadProductosMenu);
                if (productoElegido>0){
                    producto=productoElegido-1;
                    if (pedidoNuevo[producto]==0){
                        cout<<"ese producto no esta en el pedido"<<endl;
                        system("pause");
                    } else {
                        cantidadQuitar=leerEntero("cuantas unidades se quitan: ",0,pedidoNuevo[producto]);
                        pedidoNuevo[producto]-=cantidadQuitar;
                    }
                }
            } else if (opcionSubmenu==3){
                if (cuentaElegida>=cantidadMesas && estadoCuenta[cuentaElegida]==0){
                    nombreCuenta[cuentaElegida]=""; // la orden para llevar no se abrio, se borra el nombre
                }
                cout<<"pedido cancelado"<<endl;
                terminado=1;
            }
        }
    }while (terminado==0);
}

// funcion anularProducto: quita unidades de un producto de una cuenta abierta. Pide el motivo y siempre
// necesita la autorizacion de un administrador. Tambien quita esas unidades de los pedidos que
// todavia no se han entregado en cocina o barra, y deja el registro para el reporte de cierre
void anularProducto(){
    int cuenta, producto, cantidad, pedido, columna, restante, quitar, colaVacia;
    string motivo, etiqueta;
    cout<<"---------------------- anular producto ------------------------"<<endl;
    mostrarEstadoCuentas();
    cout<<endl;
    cuenta=elegirCuenta("numero de la cuenta de la que se anula un producto",1);
    if (cuenta==-1){
        return;
    }
    if (hayProductosEnCuenta(cuenta)==0){
        cout<<"la cuenta no tiene productos"<<endl;
        return;
    }
    mostrarCuenta(cuenta);
    do{
        producto=leerEntero("numero del producto a anular (0 para cancelar): ",0,cantidadProductosMenu);
        if (producto>0 && productosEnCuenta[cuenta][producto-1]==0){
            cout<<"ese producto no esta en la cuenta"<<endl;
            producto=-1;
        }
    }while (producto==-1);
    if (producto==0){
        return;
    }
    producto=producto-1;
    cantidad=leerEntero("cuantas unidades se anulan (0 para cancelar): ",0,productosEnCuenta[cuenta][producto]);
    if (cantidad==0){
        return;
    }
    motivo=leerTextoConEspacios("motivo de la anulacion: ",3,60);
    if (autorizarComoAdministrador("anular un producto")==0){
        cout<<"no se anulo el producto"<<endl;
        return;
    }
    etiqueta=etiquetaCuenta(cuenta);
    productosEnCuenta[cuenta][producto]-=cantidad;
    // se quitan las unidades de los pedidos pendientes de cocina o barra, empezando por los mas recientes
    restante=cantidad;
    for (pedido=totalEntradasCola-1;pedido>=0 && restante>0;pedido--){
        if (colaCuenta[pedido]==cuenta && colaEntregada[pedido]==0 && colaCantidad[pedido][producto]>0){
            quitar=restante;
            if (quitar>colaCantidad[pedido][producto]){
                quitar=colaCantidad[pedido][producto];
            }
            colaCantidad[pedido][producto]-=quitar;
            restante-=quitar;
            colaVacia=1; // si el pedido se quedo sin productos ya no hay nada que preparar
            for (columna=0;columna<cantidadProductosMenu;columna++){
                if (colaCantidad[pedido][columna]>0){
                    colaVacia=0;
                }
            }
            if (colaVacia==1){
                colaEntregada[pedido]=1;
            }
        }
    }
    productosAnuladosCantidad+=cantidad;
    productosAnuladosMontoCentavos+=cantidad*precioProductoCentavos[producto];
    if (cantidadRegistrosAnulacion<50){ // 50 = maximo de anulaciones que se registran al dia
        registroAnulacion[cantidadRegistrosAnulacion]=etiqueta+": "+to_string(cantidad)+" x "+nombreProducto[producto]+" ("+dinero(cantidad*precioProductoCentavos[producto])+") motivo: "+motivo+" | registrado por "+nombreUsuario[usuarioActual]+" a las "+formatearFechaHora(time(NULL),"%H:%M");
        cantidadRegistrosAnulacion++;
    }
    if (cuenta>=cantidadMesas && hayProductosEnCuenta(cuenta)==0){ // una orden para llevar sin productos se cierra sola
        estadoCuenta[cuenta]=0;
        nombreCuenta[cuenta]="";
    }
    cout<<"se anularon "<<cantidad<<" unidades de "<<nombreProducto[producto]<<" de "<<etiqueta<<endl;
}
// funcion generarPrecuentaYCobrar: muestra la precuenta de una cuenta abierta y, si se confirma,
// pide los datos del cliente (NIT y nombre), aplica un descuento opcional (con autorizacion del
// administrador), calcula el IVA incluido, pregunta la propina y cobra (se puede dividir entre varias
// personas, cada una en efectivo o con tarjeta). Al final guarda el archivo de la cuenta, acumula los
// valores del dia y libera la mesa
void generarPrecuentaYCobrar(){
    int cuenta, opcion, producto, metodo, numeroPago, personasPagan, maximoPersonas, montoPersona, unidadesPorPersona;
    int totalPagado, subtotal, descuento, neto, propina, propinaSugerida, total, pagoRealizado, iva, baseSinIva;
    string descripcionDescuento="", nit, nombreCliente;
    if (cajaAbierta==0){
        cout<<"la caja no esta abierta, el cajero debe abrirla primero (opcion 6 del menu)"<<endl;
        return;
    }
    cout<<"---------------------- precuenta y cobro ------------------------"<<endl;
    mostrarEstadoCuentas();
    cout<<endl;
    cuenta=elegirCuenta("numero de la cuenta a cobrar",1);
    if (cuenta==-1){
        return;
    }
    if (hayProductosEnCuenta(cuenta)==0){
        cout<<"la cuenta no tiene productos, si es una mesa sin consumo use 'liberar mesa sin consumo' en el menu de mesas"<<endl;
        return;
    }
    subtotal=totalCuentaCentavos(cuenta);
    cout<<endl;
    cout<<"PRECUENTA - "<<etiquetaCuenta(cuenta)<<endl;
    mostrarCuenta(cuenta);
    opcion=leerEntero("la cuenta es correcta? 1. Si, cobrar / 2. No, volver al menu: ",1,2);
    if (opcion==2){
        cout<<"la cuenta sigue abierta, puede agregar productos desde tomar orden o anular alguno"<<endl;
        return;
    }
    // datos del cliente: NIT (o CF) y nombre
    do{
        nit=leerTextoSinEspacios("NIT del cliente (escriba CF si no da NIT): ",2,13);
        if (esNitValido(nit)==0){
            cout<<"NIT no valido, escriba solo numeros (puede terminar en K) o CF"<<endl;
        }
    }while (esNitValido(nit)==0);
    if (aMinusculas(nit)=="cf"){
        nit="CF";
        nombreCliente="Consumidor Final";
    } else {
        nombreCliente=leerTextoConEspacios("nombre del cliente: ",2,40);
    }
    // descuento: un solo descuento por cuenta, en quetzales, con descripcion y autorizacion
    descuento=0;
    opcion=leerEntero("se aplicara un descuento? 1. Si / 2. No: ",1,2);
    if (opcion==1){
        if (autorizarComoAdministrador("aplicar un descuento")==1){
            descripcionDescuento=leerTextoConEspacios("descripcion del descuento: ",3,60);
            descuento=leerMontoCentavos("monto del descuento en quetzales (maximo "+dinero(subtotal)+"): ",25,subtotal,25);
        } else {
            cout<<"no se aplicara el descuento"<<endl;
        }
    }
    neto=subtotal-descuento;
    // el precio ya incluye IVA del 12%: la base sin IVA es el neto entre 1.12 (redondeado) y el IVA es la diferencia
    baseSinIva=(neto*100+56)/112;
    iva=neto-baseSinIva;
    // propina: ninguna, 10% sugerido (redondeado al quetzal) u otro monto
    propinaSugerida=((neto/10+50)/100)*100;
    opcion=leerEntero("propina: 1. Ninguna / 2. Sugerida 10% ("+dinero(propinaSugerida)+") / 3. Otro monto: ",1,3);
    propina=0;
    if (opcion==2){
        propina=propinaSugerida;
    } else if (opcion==3){
        propina=leerMontoCentavos("monto de la propina en quetzales: ",0,neto,25);
    }
    total=neto+propina;
    cout<<endl;
    cout<<"---------------------- resumen del cobro ------------------------"<<endl;
    cout<<"subtotal: "<<dinero(subtotal)<<endl;
    if (descuento>0){
        cout<<"descuento ("<<descripcionDescuento<<"): -"<<dinero(descuento)<<endl;
    }
    cout<<"total de la cuenta: "<<dinero(neto)<<" (incluye IVA de "<<dinero(iva)<<")"<<endl;
    cout<<"propina: "<<dinero(propina)<<endl;
    cout<<"TOTAL A PAGAR: "<<dinero(total)<<endl;
    opcion=leerEntero("confirmar el cobro? 1. Si / 2. No, cancelar: ",1,2);
    if (opcion==2){
        cout<<"cobro cancelado, la cuenta sigue abierta"<<endl;
        return;
    }
    // division de la cuenta: cada persona paga una parte en unidades de Q0.25 y la ultima paga lo que falte
    personasPagan=1;
    cobroCantidadPagos=0;
    if (total>0){
        maximoPersonas=total/25;
        if (maximoPersonas>20){ // 20 = maximo de personas entre las que se divide una cuenta
            maximoPersonas=20;
        }
        personasPagan=leerEntero("entre cuantas personas se divide la cuenta (1 si paga una sola persona): ",1,maximoPersonas);
        unidadesPorPersona=(total/25)/personasPagan;
        totalPagado=0;
        for (numeroPago=0;numeroPago<personasPagan;numeroPago++){
            if (numeroPago<personasPagan-1){
                montoPersona=unidadesPorPersona*25;
            } else {
                montoPersona=total-totalPagado; // la ultima persona paga lo que falta
            }
            if (personasPagan>1){
                cout<<endl<<"cobro de la persona "<<numeroPago+1<<" de "<<personasPagan<<": "<<dinero(montoPersona)<<endl;
            }
            do{ // si el efectivo no se puede completar se vuelve a preguntar la forma de pago
                metodo=leerEntero("forma de pago: 1. Efectivo / 2. Tarjeta: ",1,2);
                if (metodo==1){
                    pagoRealizado=cobrarEfectivo(montoPersona);
                } else {
                    pagoRealizado=1;
                    ultimoRecibidoCentavos=0;
                    ultimoCambioCentavos=0;
                }
            }while (pagoRealizado==0);
            cobroMetodoPago[numeroPago]=metodo;
            cobroMontoPago[numeroPago]=montoPersona;
            cobroRecibidoPago[numeroPago]=ultimoRecibidoCentavos;
            cobroCambioPago[numeroPago]=ultimoCambioCentavos;
            cobroCantidadPagos++;
            totalPagado+=montoPersona;
            if (metodo==1){
                pagosEfectivoCantidad++;
                pagosEfectivoMontoCentavos+=montoPersona;
            } else {
                pagosTarjetaCantidad++;
                pagosTarjetaMontoCentavos+=montoPersona;
            }
        }
    } else {
        cout<<"la cuenta queda en Q0.00, no se realiza ningun cobro"<<endl;
    }
    // se guardan los datos del cobro y se crea el archivo de la cuenta
    cobroNumeroCuenta=numeroSiguienteCuenta;
    cobroCuentaIndice=cuenta;
    cobroNit=nit;
    cobroNombreCliente=nombreCliente;
    cobroSubtotalCentavos=subtotal;
    cobroDescuentoCentavos=descuento;
    cobroDescripcionDescuento=descripcionDescuento;
    cobroIvaCentavos=iva;
    cobroPropinaCentavos=propina;
    cobroTotalCentavos=total;
    guardarCuentaFinal();
    // se acumulan los valores del dia para el cierre
    totalVentaBrutaCentavos+=subtotal;
    totalDescuentosCentavos+=descuento;
    totalVentaNetaCentavos+=neto;
    totalPropinasCentavos+=propina;
    totalIvaIncluidoCentavos+=iva;
    totalCobradoCentavos+=total;
    cantidadCuentasCobradas++;
    for (producto=0;producto<cantidadProductosMenu;producto++){
        unidadesVendidasHoy[producto]+=productosEnCuenta[cuenta][producto];
        productosEnCuenta[cuenta][producto]=0; // se limpia la cuenta
    }
    estadoCuenta[cuenta]=0; // y se libera la mesa o la orden para llevar
    nombreCuenta[cuenta]="";
    numeroSiguienteCuenta++;
    guardarContadorCuentas();
    cout<<endl<<"cuenta cobrada, "<<etiquetaCuenta(cuenta)<<" liberada"<<endl;
}

// funcion cobrarEfectivo: pide los billetes y monedas que entrega el cliente, verifica que alcance,
// calcula el cambio y lo da con lo que haya en la caja (del billete mas grande al mas chico).
// Si se pudo cobrar actualiza la caja y devuelve 1. Si no hay cambio exacto o se decide cambiar de
// forma de pago devuelve 0 (en ese caso la caja no se modifica).
// limitacion: el cambio se arma tomando siempre lo mas grande primero, si la caja tiene una
// combinacion distinta que si daria el cambio exacto se puede pedir al cliente billetes mas chicos
int cobrarEfectivo(int montoCentavos){
    int cantidadRecibida[8];      // 8 = denominaciones (lo que entrega el cliente)
    int disponibleParaCambio[8];  // 8 = denominaciones (lo que hay en caja mas lo que entrega el cliente)
    int cambioEntregar[8];        // 8 = denominaciones (lo que se da de cambio)
    int i, posicion, orden, totalRecibido, cambio, pendiente, cantidadPosible, opcion, cambioListo;
    do{
        cambioListo=0;
        totalRecibido=0;
        cout<<"ingrese los billetes y monedas que entrega el cliente (total a cobrar "<<dinero(montoCentavos)<<"):"<<endl;
        for (i=0;i<cantidadDenominaciones;i++){
            cantidadRecibida[i]=leerEntero("\tcantidad de "+nombreDenominacion[i]+": ",0,500);
            totalRecibido+=cantidadRecibida[i]*valorDenominacionCentavos[i];
        }
        if (totalRecibido<montoCentavos){ // validamos que alcance para pagar
            cout<<"el cliente entrego "<<dinero(totalRecibido)<<" y no alcanza para "<<dinero(montoCentavos)<<endl;
        } else {
            cambio=totalRecibido-montoCentavos;
            pendiente=cambio; // lo que falta por dar de cambio
            for (i=0;i<cantidadDenominaciones;i++){
                disponibleParaCambio[i]=cajaActual[i]+cantidadRecibida[i]; // el dinero del cliente tambien queda disponible para el cambio
                cambioEntregar[i]=0;
            }
            for (orden=0;orden<cantidadDenominaciones;orden++){ // se da el cambio de mayor a menor valor
                posicion=ordenDeCambio[orden];
                cantidadPosible=pendiente/valorDenominacionCentavos[posicion]; // cuantas caben en lo pendiente
                if (cantidadPosible>disponibleParaCambio[posicion]){ // no se pueden dar mas de las que hay
                    cantidadPosible=disponibleParaCambio[posicion];
                }
                cambioEntregar[posicion]=cantidadPosible;
                pendiente=pendiente-cantidadPosible*valorDenominacionCentavos[posicion];
            }
            if (pendiente>0){ // si sobro algo, la caja no puede dar el cambio exacto
                cout<<"la caja no tiene billetes y monedas para dar el cambio exacto de "<<dinero(cambio)<<endl;
            } else {
                for (i=0;i<cantidadDenominaciones;i++){
                    cajaActual[i]=disponibleParaCambio[i]-cambioEntregar[i]; // se actualiza la caja
                }
                cout<<"cambio a entregar: "<<dinero(cambio)<<endl;
                for (i=0;i<cantidadDenominaciones;i++){
                    if (cambioEntregar[i]>0){
                        cout<<"\t"<<cambioEntregar[i]<<" "<<nombreDenominacion[i]<<endl;
                    }
                }
                ultimoRecibidoCentavos=totalRecibido; // se guardan para el archivo de la cuenta
                ultimoCambioCentavos=cambio;
                cambioListo=1;
            }
        }
        if (cambioListo==0){ // si no se pudo cobrar se pregunta que hacer
            opcion=leerEntero("1. volver a ingresar lo que entrega el cliente / 2. cambiar forma de pago: ",1,2);
            if (opcion==2){
                return 0;
            }
        }
    }while (cambioListo==0);
    return 1;
}

// funcion guardarCuentaFinal: crea el archivo .txt de la cuenta cobrada (cuenta_0001.txt, cuenta_0002.txt...)
// con la fecha, quien atendio, el cliente, los productos, el descuento, el IVA, la propina y los pagos
void guardarCuentaFinal(){
    int producto, numeroPago;
    string nombreArchivo="cuenta_"+numeroConCeros(cobroNumeroCuenta,4)+".txt";
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()){
        cout<<"no se pudo crear el archivo de la cuenta"<<endl;
        return;
    }
    archivo<<"RESTAURANTE"<<endl;
    archivo<<"cuenta no. "<<numeroConCeros(cobroNumeroCuenta,4)<<endl;
    archivo<<"fecha y hora: "<<formatearFechaHora(time(NULL),"%Y-%m-%d %H:%M:%S")<<endl;
    archivo<<"atendio: "<<nombreCompletoUsuario[usuarioActual]<<" ("<<nombreRol[rolActual]<<")"<<endl;
    archivo<<"cuenta: "<<etiquetaCuenta(cobroCuentaIndice)<<endl;
    archivo<<"NIT: "<<cobroNit<<endl;
    archivo<<"cliente: "<<cobroNombreCliente<<endl;
    archivo<<"----------------------------------------"<<endl;
    for (producto=0;producto<cantidadProductosMenu;producto++){ // solo se escriben los productos que si se pidieron
        if (productosEnCuenta[cobroCuentaIndice][producto]>0){
            archivo<<productosEnCuenta[cobroCuentaIndice][producto]<<" x "<<nombreProducto[producto]<<" ("<<dinero(precioProductoCentavos[producto])<<" c/u) = "<<dinero(productosEnCuenta[cobroCuentaIndice][producto]*precioProductoCentavos[producto])<<endl;
        }
    }
    archivo<<"----------------------------------------"<<endl;
    archivo<<"subtotal: "<<dinero(cobroSubtotalCentavos)<<endl;
    if (cobroDescuentoCentavos>0){
        archivo<<"descuento: -"<<dinero(cobroDescuentoCentavos)<<endl;
        archivo<<"descripcion del descuento: "<<cobroDescripcionDescuento<<endl;
    }
    archivo<<"total de la cuenta: "<<dinero(cobroSubtotalCentavos-cobroDescuentoCentavos)<<endl;
    archivo<<"IVA 12% incluido en el total: "<<dinero(cobroIvaCentavos)<<endl;
    archivo<<"propina: "<<dinero(cobroPropinaCentavos)<<endl;
    archivo<<"TOTAL PAGADO: "<<dinero(cobroTotalCentavos)<<endl;
    archivo<<"----------------------------------------"<<endl;
    if (cobroCantidadPagos==0){
        archivo<<"forma de pago: sin cobro"<<endl;
    }
    for (numeroPago=0;numeroPago<cobroCantidadPagos;numeroPago++){
        if (cobroCantidadPagos>1){
            archivo<<"pago "<<numeroPago+1<<" de "<<cobroCantidadPagos<<": ";
        } else {
            archivo<<"pago: ";
        }
        if (cobroMetodoPago[numeroPago]==1){
            archivo<<"efectivo "<<dinero(cobroMontoPago[numeroPago])<<" (recibido "<<dinero(cobroRecibidoPago[numeroPago])<<", cambio "<<dinero(cobroCambioPago[numeroPago])<<")"<<endl;
        } else {
            archivo<<"tarjeta "<<dinero(cobroMontoPago[numeroPago])<<endl;
        }
    }
    archivo<<endl;
    archivo<<"este comprobante no es una factura legal"<<endl;
    archivo.close();
    cout<<"se genero el archivo "<<nombreArchivo<<endl;
}
// funcion menuCola: submenu para elegir si se atiende la cola de cocina o la de barra
void menuCola(){
    int opcion;
    do{
        system("cls");
        cout<<"---------------------- cola de preparacion ------------------------"<<endl;
        cout<<"1. Cocina (pizzas y otros)"<<endl;
        cout<<"2. Barra (bebidas)"<<endl;
        cout<<"0. Volver al menu principal"<<endl;
        opcion=leerEntero("seleccione una opcion: ",0,2);
        if (opcion!=0){
            atenderColaDestino(opcion);
        }
    }while (opcion!=0);
}

// funcion atenderColaDestino: muestra los pedidos pendientes de cocina (destino 1) o barra (destino 2),
// del mas antiguo al mas nuevo, con la hora y los minutos que llevan esperando (los de 15 minutos o mas
// se marcan como atrasados), y permite escoger cuales ya se entregaron
void atenderColaDestino(int destino){
    int pedido, producto, pendientes, numeroPedido, pedidoValido, minutosEspera;
    time_t ahora;
    do{
        system("cls");
        ahora=time(NULL);
        pendientes=0;
        if (destino==1){
            cout<<"---------------------- COCINA: pedidos pendientes ------------------------"<<endl;
        } else {
            cout<<"---------------------- BARRA: pedidos pendientes ------------------------"<<endl;
        }
        for (pedido=0;pedido<totalEntradasCola;pedido++){
            if (colaDestino[pedido]==destino && colaEntregada[pedido]==0){
                pendientes++;
                minutosEspera=(int)(difftime(ahora,colaHoraTomada[pedido])/60);
                cout<<"Pedido no."<<pedido+1<<" | "<<etiquetaCuenta(colaCuenta[pedido])<<" | hora "<<formatearFechaHora(colaHoraTomada[pedido],"%H:%M")<<" | espera "<<minutosEspera<<" min";
                if (minutosEspera>=15){
                    cout<<"  <-- ATRASADO";
                }
                cout<<endl;
                for (producto=0;producto<cantidadProductosMenu;producto++){
                    if (colaCantidad[pedido][producto]>0){
                        cout<<"\t"<<colaCantidad[pedido][producto]<<" x "<<nombreProducto[producto]<<endl;
                    }
                }
            }
        }
        if (pendientes==0){
            cout<<"no hay pedidos pendientes"<<endl;
            system("pause");
            numeroPedido=0;
        } else {
            do{
                pedidoValido=1;
                numeroPedido=leerEntero("numero del pedido que ya se entrego (0 para volver): ",0,totalEntradasCola);
                if (numeroPedido!=0 && (colaDestino[numeroPedido-1]!=destino || colaEntregada[numeroPedido-1]==1)){
                    cout<<"ese pedido no esta pendiente en esta cola"<<endl;
                    pedidoValido=0;
                }
            }while (pedidoValido==0);
            if (numeroPedido!=0){
                colaEntregada[numeroPedido-1]=1;
                cout<<"el pedido no."<<numeroPedido<<" fue marcado como entregado"<<endl;
                system("pause");
            }
        }
    }while (numeroPedido!=0);
}
// funcion sumaDenominaciones: recibe cuantos billetes y monedas hay de cada tipo y devuelve
// el dinero total en centavos
int sumaDenominaciones(int cantidades[]){
    int i, total=0;
    for (i=0;i<cantidadDenominaciones;i++){
        total+=cantidades[i]*valorDenominacionCentavos[i];
    }
    return total;
}

// funcion aperturaCaja: abre la caja del dia. Si el dia anterior se guardo lo que se dejo en caja,
// ofrece usar esos mismos valores; si no, pide cuantos billetes y monedas hay y los confirma.
// Guarda quien abrio la caja y a que hora
void aperturaCaja(){
    int i, opcion, usarGuardados=0, hayGuardados=0;
    long long numerosLeidos[8]; // 8 = denominaciones (valores leidos de caja_dejada.txt)
    int cantidadesGuardadas[8]; // 8 = denominaciones
    string linea;
    cout<<"---------------------- apertura de caja ------------------------"<<endl;
    if (cajaAbierta==1){
        cout<<"la caja ya esta abierta (la abrio "<<usuarioApertura<<" a las "<<horaApertura<<")"<<endl;
        return;
    }
    ifstream archivoDejado("caja_dejada.txt"); // lo que quedo en caja al cerrar el dia anterior
    if (archivoDejado.is_open()){
        linea=leerLineaArchivo(archivoDejado);
        if (separarNumeros(linea,numerosLeidos,8)==cantidadDenominaciones){
            hayGuardados=1;
            for (i=0;i<cantidadDenominaciones;i++){
                cantidadesGuardadas[i]=(int)numerosLeidos[i];
            }
        }
        archivoDejado.close();
    }
    if (hayGuardados==1){
        cout<<"el dia anterior se dejaron en caja ("<<dinero(sumaDenominaciones(cantidadesGuardadas))<<"):"<<endl;
        for (i=0;i<cantidadDenominaciones;i++){
            cout<<"\t"<<cantidadesGuardadas[i]<<" "<<nombreDenominacion[i]<<endl;
        }
        opcion=leerEntero("1. usar estos valores / 2. contar e ingresar los valores manualmente: ",1,2);
        if (opcion==1){
            usarGuardados=1;
            for (i=0;i<cantidadDenominaciones;i++){
                cajaApertura[i]=cantidadesGuardadas[i];
            }
        }
    }
    if (usarGuardados==0){
        do{
            cout<<"ingrese la cantidad de billetes y monedas que hay en caja:"<<endl;
            for (i=0;i<cantidadDenominaciones;i++){
                cajaApertura[i]=leerEntero("\tcantidad de "+nombreDenominacion[i]+": ",0,5000);
            }
            cout<<"total ingresado: "<<dinero(sumaDenominaciones(cajaApertura))<<endl;
            opcion=leerEntero("los valores son correctos? 1. Si / 2. No, ingresar de nuevo: ",1,2);
        }while (opcion!=1);
    }
    for (i=0;i<cantidadDenominaciones;i++){
        cajaActual[i]=cajaApertura[i]; // la caja empieza con lo de la apertura
    }
    cajaAbierta=1;
    usuarioApertura=nombreUsuario[usuarioActual];
    horaApertura=formatearFechaHora(time(NULL),"%Y-%m-%d %H:%M:%S");
    cout<<"caja abierta con "<<dinero(sumaDenominaciones(cajaApertura))<<endl;
}

// funcion mostrarEstadoCaja: muestra cuantos billetes y monedas hay ahora en la caja, el total
// y un resumen de lo que ha pasado en el dia (cobros en efectivo, retiros y gastos)
void mostrarEstadoCaja(){
    int i;
    cout<<"---------------------- estado de la caja ------------------------"<<endl;
    if (cajaAbierta==0){
        cout<<"la caja no esta abierta"<<endl;
        return;
    }
    cout<<"abierta por "<<usuarioApertura<<" a las "<<horaApertura<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        cout<<"\t"<<cajaActual[i]<<" "<<nombreDenominacion[i]<<endl;
    }
    cout<<"efectivo en caja ahora: "<<dinero(sumaDenominaciones(cajaActual))<<endl;
    cout<<"efectivo con el que se abrio: "<<dinero(sumaDenominaciones(cajaApertura))<<endl;
    cout<<"cobrado en efectivo en el dia: "<<dinero(pagosEfectivoMontoCentavos)<<endl;
    cout<<"cobrado con tarjeta en el dia: "<<dinero(pagosTarjetaMontoCentavos)<<endl;
    cout<<"retiros: "<<dinero(totalRetirosCentavos)<<" | gastos: "<<dinero(totalGastosCentavos)<<endl;
}

// funcion registrarMovimientoCaja: registra un retiro de efectivo o un gasto pagado desde la caja.
// necesita la autorizacion de un administrador, una descripcion y cuantos billetes y monedas se sacan
// (no se puede sacar mas de lo que hay). Queda anotado para el cierre
void registrarMovimientoCaja(){
    int tipo, i, totalMovimiento=0;
    int cantidadesRetiradas[8]; // 8 = denominaciones (billetes y monedas que se sacan de la caja)
    string descripcion;
    cout<<"---------------------- retiro o gasto de caja ------------------------"<<endl;
    if (cajaAbierta==0){
        cout<<"la caja no esta abierta"<<endl;
        return;
    }
    if (cantidadMovimientosCaja>=50){ // 50 = maximo de movimientos de caja que se registran al dia
        cout<<"se alcanzo el maximo de movimientos de caja del dia"<<endl;
        return;
    }
    tipo=leerEntero("tipo: 1. Retiro de efectivo / 2. Gasto pagado desde la caja / 0. Cancelar: ",0,2);
    if (tipo==0){
        return;
    }
    if (autorizarComoAdministrador("registrar un movimiento de caja")==0){
        return;
    }
    descripcion=leerTextoConEspacios("descripcion: ",3,60);
    cout<<"ingrese los billetes y monedas que se sacan de la caja:"<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        cantidadesRetiradas[i]=leerEntero("\tcantidad de "+nombreDenominacion[i]+" (hay "+to_string(cajaActual[i])+"): ",0,cajaActual[i]);
    }
    totalMovimiento=sumaDenominaciones(cantidadesRetiradas);
    if (totalMovimiento==0){
        cout<<"no se saco dinero, no se registro ningun movimiento"<<endl;
        return;
    }
    for (i=0;i<cantidadDenominaciones;i++){
        cajaActual[i]-=cantidadesRetiradas[i];
    }
    movimientoTipo[cantidadMovimientosCaja]=tipo;
    movimientoDescripcion[cantidadMovimientosCaja]=descripcion;
    movimientoMontoCentavos[cantidadMovimientosCaja]=totalMovimiento;
    movimientoUsuario[cantidadMovimientosCaja]=nombreUsuario[usuarioActual];
    movimientoHora[cantidadMovimientosCaja]=formatearFechaHora(time(NULL),"%H:%M");
    cantidadMovimientosCaja++;
    if (tipo==1){
        totalRetirosCentavos+=totalMovimiento;
        cout<<"retiro registrado por "<<dinero(totalMovimiento)<<endl;
    } else {
        totalGastosCentavos+=totalMovimiento;
        cout<<"gasto registrado por "<<dinero(totalMovimiento)<<endl;
    }
}

// =====================================================================
// CIERRE DEL DIA
// =====================================================================

// funcion escribirReporteCierre: escribe el reporte del cierre del dia en el lugar que se le indique.
// se llama dos veces: una con cout (para verlo en pantalla) y otra con un archivo (para guardarlo),
// asi el reporte es exactamente el mismo en los dos lugares
void escribirReporteCierre(ostream &salida){
    int i;
    int diferenciaCentavos=cierreContadoCentavos-cierreEsperadoCentavos; // positivo = sobrante, negativo = faltante
    salida<<"=================================================================="<<endl;
    salida<<"                    REPORTE DE CIERRE DEL DIA"<<endl;
    salida<<"=================================================================="<<endl;
    salida<<"caja abierta por: "<<usuarioApertura<<" a las "<<horaApertura<<endl;
    salida<<"dia cerrado por:  "<<usuarioCierre<<" a las "<<horaCierre<<endl;
    salida<<endl;
    salida<<"------------------------ PRODUCTOS VENDIDOS ----------------------"<<endl;
    int huboVentas=0; // 0 mientras no se haya escrito ningun producto vendido
    for (i=0;i<cantidadProductosMenu;i++){
        if (unidadesVendidasHoy[i]>0){
            salida<<unidadesVendidasHoy[i]<<" x "<<nombreProducto[i]<<endl;
            huboVentas=1;
        }
    }
    if (huboVentas==0){
        salida<<"no se vendio ningun producto"<<endl;
    }
    salida<<endl;
    salida<<"------------------------------ VENTAS ----------------------------"<<endl;
    salida<<"cuentas cobradas:           "<<cantidadCuentasCobradas<<endl;
    salida<<"venta bruta:                "<<dinero(totalVentaBrutaCentavos)<<endl;
    salida<<"descuentos:                 "<<dinero(totalDescuentosCentavos)<<endl;
    salida<<"venta neta:                 "<<dinero(totalVentaNetaCentavos)<<endl;
    salida<<"IVA 12% (ya incluido):      "<<dinero(totalIvaIncluidoCentavos)<<endl;
    salida<<"propinas:                   "<<dinero(totalPropinasCentavos)<<endl;
    salida<<"total cobrado:              "<<dinero(totalCobradoCentavos)<<endl;
    salida<<endl;
    salida<<"-------------------------- FORMAS DE PAGO ------------------------"<<endl;
    salida<<"efectivo: "<<pagosEfectivoCantidad<<" pagos por "<<dinero(pagosEfectivoMontoCentavos)<<endl;
    salida<<"tarjeta:  "<<pagosTarjetaCantidad<<" pagos por "<<dinero(pagosTarjetaMontoCentavos)<<endl;
    salida<<endl;
    salida<<"-------------------------- RETIROS Y GASTOS ----------------------"<<endl;
    if (cantidadMovimientosCaja==0){
        salida<<"no hubo retiros ni gastos"<<endl;
    }
    for (i=0;i<cantidadMovimientosCaja;i++){
        if (movimientoTipo[i]==1){
            salida<<"retiro ";
        }else{
            salida<<"gasto  ";
        }
        salida<<dinero(movimientoMontoCentavos[i])<<" - "<<movimientoDescripcion[i]<<" ("<<movimientoUsuario[i]<<", "<<movimientoHora[i]<<")"<<endl;
    }
    salida<<"total retiros: "<<dinero(totalRetirosCentavos)<<" | total gastos: "<<dinero(totalGastosCentavos)<<endl;
    salida<<endl;
    salida<<"---------------------------- ANULACIONES -------------------------"<<endl;
    if (cantidadRegistrosAnulacion==0){
        salida<<"no hubo productos anulados"<<endl;
    }
    for (i=0;i<cantidadRegistrosAnulacion;i++){
        salida<<registroAnulacion[i]<<endl;
    }
    salida<<"unidades anuladas: "<<productosAnuladosCantidad<<" por "<<dinero(productosAnuladosMontoCentavos)<<endl;
    salida<<endl;
    salida<<"------------------------- CUADRE DE LA CAJA ----------------------"<<endl;
    salida<<"efectivo con el que se abrio:   "<<dinero(sumaDenominaciones(cajaApertura))<<endl;
    salida<<"efectivo esperado en caja:      "<<dinero(cierreEsperadoCentavos)<<endl;
    salida<<"efectivo contado por el cajero: "<<dinero(cierreContadoCentavos)<<endl;
    if (diferenciaCentavos==0){
        salida<<"resultado: la caja cuadra exacto"<<endl;
    }else if (diferenciaCentavos<0){
        salida<<"resultado: FALTANTE de "<<dinero(-diferenciaCentavos)<<endl;
    }else{
        salida<<"resultado: SOBRANTE de "<<dinero(diferenciaCentavos)<<endl;
    }
    salida<<"detalle por billete y moneda (esperado / contado):"<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        salida<<"\t"<<nombreDenominacion[i]<<": "<<cajaActual[i]<<" / "<<cierreConteoCaja[i];
        if (cajaActual[i]!=cierreConteoCaja[i]){
            salida<<"  <-- diferencia";
        }
        salida<<endl;
    }
    salida<<endl;
    salida<<"-------------------------- DESTINO DEL EFECTIVO ------------------"<<endl;
    salida<<"se deja en caja para manana ("<<dinero(sumaDenominaciones(cierreDejarEnCaja))<<"):"<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        salida<<"\t"<<cierreDejarEnCaja[i]<<" "<<nombreDenominacion[i]<<endl;
    }
    salida<<"efectivo que se lleva a casa: "<<dinero(cierreEfectivoCasaCentavos)<<endl;
    salida<<"=================================================================="<<endl;
}

// funcion finalizarDia: cierra el dia. Primero revisa que no haya cuentas abiertas ni pedidos sin entregar.
// Despues el cajero cuenta el efectivo, se calcula faltante o sobrante, se muestra el reporte,
// se guarda en un archivo cierre_fecha_hora.txt, se guarda lo que queda en caja para el dia siguiente
// y se borra el estado temporal del dia. Al terminar el programa se cierra (diaFinalizado=1)
void finalizarDia(){
    int i, opcion, confirmado=0;
    int cuentasAbiertas=0;     // cuentas con consumo sin cobrar
    int reservas=0;            // mesas reservadas (se pierden al cerrar el dia)
    int pedidosPendientes=0;   // pedidos de cocina o barra sin entregar
    cout<<"---------------------- finalizar el dia ------------------------"<<endl;
    for (i=0;i<cantidadCuentas;i++){
        if (estadoCuenta[i]==1){
            cuentasAbiertas++;
        }
        if (estadoCuenta[i]==2){
            reservas++;
        }
    }
    for (i=0;i<totalEntradasCola;i++){
        if (colaEntregada[i]==0){
            pedidosPendientes++;
        }
    }
    if (cuentasAbiertas>0 || pedidosPendientes>0){
        cout<<"no se puede finalizar el dia todavia:"<<endl;
        if (cuentasAbiertas>0){
            cout<<"\t- hay "<<cuentasAbiertas<<" cuenta(s) abierta(s) sin cobrar"<<endl;
        }
        if (pedidosPendientes>0){
            cout<<"\t- hay "<<pedidosPendientes<<" pedido(s) en cocina o barra sin entregar"<<endl;
        }
        cout<<"cobre o anule las cuentas y entregue los pedidos primero"<<endl;
        return;
    }
    if (reservas>0){
        cout<<"aviso: hay "<<reservas<<" mesa(s) reservada(s); las reservas se perderan al cerrar el dia"<<endl;
    }
    if (cajaAbierta==0){
        // la caja nunca se abrio, asi que no hay nada que cuadrar
        cout<<"la caja no se abrio en este dia, no hay cierre de caja que hacer"<<endl;
        opcion=leerEntero("desea salir del programa sin cierre? 1. Si / 2. No: ",1,2);
        if (opcion==1){
            remove("estado.txt");
            remove("estado.tmp");
            diaFinalizado=1;
        }
        return;
    }
    opcion=leerEntero("esta seguro de que desea finalizar el dia? 1. Si / 2. No: ",1,2);
    if (opcion==2){
        cout<<"cierre cancelado"<<endl;
        return;
    }
    // el cajero cuenta lo que hay fisicamente en la caja
    do{
        cout<<"cuente el efectivo de la caja e ingrese cuantos billetes y monedas hay:"<<endl;
        for (i=0;i<cantidadDenominaciones;i++){
            cierreConteoCaja[i]=leerEntero("\tcantidad de "+nombreDenominacion[i]+": ",0,5000);
        }
        cout<<"total contado: "<<dinero(sumaDenominaciones(cierreConteoCaja))<<endl;
        opcion=leerEntero("los valores son correctos? 1. Si / 2. No, contar de nuevo: ",1,2);
        if (opcion==1){
            confirmado=1;
        }
    }while (confirmado==0);
    cierreEsperadoCentavos=sumaDenominaciones(cajaActual);
    cierreContadoCentavos=sumaDenominaciones(cierreConteoCaja);
    // se deja en caja para manana lo mismo que habia al abrir, pero nunca mas de lo que hay contado
    for (i=0;i<cantidadDenominaciones;i++){
        if (cierreConteoCaja[i]<cajaApertura[i]){
            cierreDejarEnCaja[i]=cierreConteoCaja[i];
        }else{
            cierreDejarEnCaja[i]=cajaApertura[i];
        }
    }
    cierreEfectivoCasaCentavos=cierreContadoCentavos-sumaDenominaciones(cierreDejarEnCaja);
    usuarioCierre=nombreUsuario[usuarioActual];
    horaCierre=formatearFechaHora(time(NULL),"%Y-%m-%d %H:%M:%S");
    system("cls");
    escribirReporteCierre(cout);
    // reporte en archivo
    string nombreArchivoCierre="cierre_"+formatearFechaHora(time(NULL),"%Y%m%d_%H%M%S")+".txt";
    ofstream archivoCierre(nombreArchivoCierre.c_str());
    if (archivoCierre.is_open()){
        escribirReporteCierre(archivoCierre);
        archivoCierre.close();
        cout<<"el reporte se guardo en el archivo "<<nombreArchivoCierre<<endl;
    }else{
        cout<<"no se pudo guardar el archivo del reporte, tome nota del reporte de pantalla"<<endl;
    }
    // lo que queda en caja se guarda para ofrecerlo al abrir la caja el dia siguiente
    ofstream archivoDejado("caja_dejada.txt");
    if (archivoDejado.is_open()){
        for (i=0;i<cantidadDenominaciones;i++){
            archivoDejado<<cierreDejarEnCaja[i]<<" ";
        }
        archivoDejado<<endl;
        archivoDejado.close();
    }
    // el dia termino: ya no se necesita el estado temporal
    remove("estado.txt");
    remove("estado.tmp");
    diaFinalizado=1;
}

// =====================================================================
// ARCHIVOS (menu, contador de cuentas y estado del dia)
// =====================================================================

// funcion leerLineaArchivo: lee una linea de un archivo y le quita los espacios de los extremos.
// si ya no hay mas lineas devuelve un texto vacio
string leerLineaArchivo(ifstream &archivo){
    string linea="";
    getline(archivo,linea);
    return recortarEspacios(linea);
}

// funcion separarNumeros: toma una linea como "5 10 -3 7" y guarda cada numero en el arreglo destino.
// primero llena el arreglo con ceros (posiciones 0 a maximo-1). Devuelve cuantos numeros encontro
int separarNumeros(string linea, long long destino[], int maximo){
    int i, encontrados=0;
    int hayNumero=0;      // 1 mientras se esta leyendo un numero
    int esNegativo=0;     // 1 si el numero que se esta leyendo empieza con -
    int cantidadDigitos=0;// digitos leidos del numero actual
    long long numeroActual=0;
    for (i=0;i<maximo;i++){
        destino[i]=0;
    }
    for (i=0;i<=(int)linea.length();i++){
        char letra;
        if (i<(int)linea.length()){
            letra=linea[i];
        }else{
            letra=' '; // al final de la linea se cierra el ultimo numero
        }
        if (letra>='0' && letra<='9'){
            if (hayNumero==0){
                hayNumero=1;
                numeroActual=0;
                cantidadDigitos=0;
            }
            if (cantidadDigitos<18){ // 18 = digitos maximos para que quepa en un long long
                numeroActual=numeroActual*10+(letra-'0');
                cantidadDigitos++;
            }
        }else{
            if (hayNumero==1){
                if (esNegativo==1){
                    numeroActual=-numeroActual;
                }
                if (encontrados<maximo){
                    destino[encontrados]=numeroActual;
                }
                encontrados++;
                hayNumero=0;
                esNegativo=0;
            }
            if (letra=='-'){
                esNegativo=1;
            }else{
                esNegativo=0;
            }
        }
    }
    return encontrados;
}

// funcion cargarMenu: lee el menu desde menu.txt. Si el archivo no existe o esta danado,
// crea el menu por defecto y lo guarda
void cargarMenu(){
    int i, leidoCorrecto=0;
    long long numerosLeidos[5]; // 5 = precio, categoria, agotado, activo (4 valores, uno de sobra)
    string linea;
    ifstream archivoMenu("menu.txt");
    cantidadProductosMenu=0;
    if (archivoMenu.is_open()){
        linea=leerLineaArchivo(archivoMenu);
        if (linea=="MENU_V1"){
            linea=leerLineaArchivo(archivoMenu);
            int cantidadEnArchivo=textoAEntero(linea);
            if (cantidadEnArchivo>=1 && cantidadEnArchivo<=40){ // 40 = maximo de productos del menu
                leidoCorrecto=1;
                for (i=0;i<cantidadEnArchivo;i++){
                    nombreProducto[i]=leerLineaArchivo(archivoMenu);
                    linea=leerLineaArchivo(archivoMenu);
                    separarNumeros(linea,numerosLeidos,4); // 4 = precio, categoria, agotado y activo
                    precioProductoCentavos[i]=(int)numerosLeidos[0];
                    categoriaProducto[i]=(int)numerosLeidos[1];
                    productoAgotado[i]=(int)numerosLeidos[2];
                    productoActivo[i]=(int)numerosLeidos[3];
                    if (nombreProducto[i]=="" || precioProductoCentavos[i]<=0 || categoriaProducto[i]<1 || categoriaProducto[i]>cantidadCategorias){
                        leidoCorrecto=0; // algun producto esta danado
                    }
                }
                if (leidoCorrecto==1){
                    cantidadProductosMenu=cantidadEnArchivo;
                }
            }
        }
        archivoMenu.close();
    }
    if (leidoCorrecto==0){
        cantidadProductosMenu=0;
        crearMenuPorDefecto();
        guardarMenu();
    }
}

// funcion guardarMenu: guarda el menu completo en menu.txt (nombre en una linea y sus datos en la siguiente)
void guardarMenu(){
    int i;
    ofstream archivoMenu("menu.txt");
    if (!archivoMenu.is_open()){
        return; // si no se puede guardar el programa sigue funcionando con el menu en memoria
    }
    archivoMenu<<"MENU_V1"<<endl;
    archivoMenu<<cantidadProductosMenu<<endl;
    for (i=0;i<cantidadProductosMenu;i++){
        archivoMenu<<nombreProducto[i]<<endl;
        archivoMenu<<precioProductoCentavos[i]<<" "<<categoriaProducto[i]<<" "<<productoAgotado[i]<<" "<<productoActivo[i]<<endl;
    }
    archivoMenu.close();
}

// funcion cargarContadorCuentas: lee en contador_cuentas.txt cual es el numero de la proxima cuenta.
// si no existe el archivo, empieza en 1
void cargarContadorCuentas(){
    string linea;
    int numeroLeido;
    numeroSiguienteCuenta=1;
    ifstream archivoContador("contador_cuentas.txt");
    if (archivoContador.is_open()){
        linea=leerLineaArchivo(archivoContador);
        numeroLeido=textoAEntero(linea);
        if (numeroLeido>=1 && numeroLeido<=9999){ // 9999 = ultimo numero que cabe en cuenta_0001.txt (4 digitos)
            numeroSiguienteCuenta=numeroLeido;
        }
        archivoContador.close();
    }
}

// funcion guardarContadorCuentas: guarda en contador_cuentas.txt el numero de la proxima cuenta
// para que la numeracion continue aunque se cierre el programa
void guardarContadorCuentas(){
    ofstream archivoContador("contador_cuentas.txt");
    if (archivoContador.is_open()){
        archivoContador<<numeroSiguienteCuenta<<endl;
        archivoContador.close();
    }
}

// funcion guardarEstado: guarda en estado.txt todo lo que ha pasado en el dia (caja, cuentas, cola,
// acumulados, retiros y anulaciones) para poder recuperarlo si el programa se cierra por error.
// escribe primero en estado.tmp y luego lo renombra, asi nunca queda un estado.txt a medias
void guardarEstado(){
    int i, j;
    ofstream archivoEstado("estado.tmp");
    if (!archivoEstado.is_open()){
        return; // si no se puede guardar el programa sigue funcionando normalmente
    }
    archivoEstado<<"ESTADO_V1"<<endl;
    archivoEstado<<formatearFechaHora(time(NULL),"%Y-%m-%d")<<endl;
    archivoEstado<<cajaAbierta<<endl;
    archivoEstado<<usuarioApertura<<endl;
    archivoEstado<<horaApertura<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        archivoEstado<<cajaApertura[i]<<" ";
    }
    archivoEstado<<endl;
    for (i=0;i<cantidadDenominaciones;i++){
        archivoEstado<<cajaActual[i]<<" ";
    }
    archivoEstado<<endl;
    archivoEstado<<cantidadProductosMenu<<endl;
    // cuentas (mesas y para llevar)
    for (i=0;i<cantidadCuentas;i++){
        archivoEstado<<estadoCuenta[i]<<endl;
        archivoEstado<<nombreCuenta[i]<<endl;
        for (j=0;j<cantidadProductosMenu;j++){
            archivoEstado<<productosEnCuenta[i][j]<<" ";
        }
        archivoEstado<<endl;
    }
    // cola de cocina y barra
    archivoEstado<<totalEntradasCola<<endl;
    for (i=0;i<totalEntradasCola;i++){
        archivoEstado<<colaCuenta[i]<<" "<<colaDestino[i]<<" "<<colaEntregada[i]<<" "<<(long long)colaHoraTomada[i]<<endl;
        for (j=0;j<cantidadProductosMenu;j++){
            archivoEstado<<colaCantidad[i][j]<<" ";
        }
        archivoEstado<<endl;
    }
    // acumulados del dia (15 numeros, el orden debe ser el mismo en cargarEstado)
    archivoEstado<<totalVentaBrutaCentavos<<" "<<totalDescuentosCentavos<<" "<<totalVentaNetaCentavos<<" "<<totalPropinasCentavos<<" "<<totalIvaIncluidoCentavos<<" "<<totalCobradoCentavos<<" "<<cantidadCuentasCobradas<<" ";
    archivoEstado<<pagosEfectivoCantidad<<" "<<pagosEfectivoMontoCentavos<<" "<<pagosTarjetaCantidad<<" "<<pagosTarjetaMontoCentavos<<" ";
    archivoEstado<<totalRetirosCentavos<<" "<<totalGastosCentavos<<" "<<productosAnuladosCantidad<<" "<<productosAnuladosMontoCentavos<<endl;
    for (j=0;j<cantidadProductosMenu;j++){
        archivoEstado<<unidadesVendidasHoy[j]<<" ";
    }
    archivoEstado<<endl;
    // retiros y gastos
    archivoEstado<<cantidadMovimientosCaja<<endl;
    for (i=0;i<cantidadMovimientosCaja;i++){
        archivoEstado<<movimientoTipo[i]<<" "<<movimientoMontoCentavos[i]<<endl;
        archivoEstado<<movimientoDescripcion[i]<<endl;
        archivoEstado<<movimientoUsuario[i]<<endl;
        archivoEstado<<movimientoHora[i]<<endl;
    }
    // anulaciones
    archivoEstado<<cantidadRegistrosAnulacion<<endl;
    for (i=0;i<cantidadRegistrosAnulacion;i++){
        archivoEstado<<registroAnulacion[i]<<endl;
    }
    archivoEstado<<"FIN_ESTADO"<<endl;
    archivoEstado.close();
    remove("estado.txt");
    rename("estado.tmp","estado.txt");
}

// funcion cargarEstado: lee estado.txt (si existe) y recupera lo que habia pasado en el dia.
// funciona igual que guardarEstado pero al reves. Si habia actividad avisa que se recupero
void cargarEstado(){
    int i, j, hayActividad=0;
    long long numerosLeidos[60]; // 60 = mas que cualquier linea del archivo (la mas larga tiene 40 numeros)
    string linea;
    ifstream archivoEstado("estado.txt");
    if (!archivoEstado.is_open()){
        return; // no hay estado guardado: es un dia nuevo
    }
    linea=leerLineaArchivo(archivoEstado);
    if (linea!="ESTADO_V1"){
        archivoEstado.close();
        return; // archivo que no es de este programa
    }
    leerLineaArchivo(archivoEstado); // fecha (solo informativa)
    linea=leerLineaArchivo(archivoEstado);
    cajaAbierta=textoAEntero(linea);
    if (cajaAbierta!=1){
        cajaAbierta=0;
    }
    usuarioApertura=leerLineaArchivo(archivoEstado);
    horaApertura=leerLineaArchivo(archivoEstado);
    linea=leerLineaArchivo(archivoEstado);
    separarNumeros(linea,numerosLeidos,60); // 60 = tamano del arreglo de lectura
    for (i=0;i<cantidadDenominaciones;i++){
        cajaApertura[i]=(int)numerosLeidos[i];
    }
    linea=leerLineaArchivo(archivoEstado);
    separarNumeros(linea,numerosLeidos,60);
    for (i=0;i<cantidadDenominaciones;i++){
        cajaActual[i]=(int)numerosLeidos[i];
    }
    leerLineaArchivo(archivoEstado); // cantidad de productos del menu cuando se guardo (el menu real se lee de menu.txt)
    // cuentas
    for (i=0;i<cantidadCuentas;i++){
        linea=leerLineaArchivo(archivoEstado);
        estadoCuenta[i]=textoAEntero(linea);
        if (estadoCuenta[i]<0 || estadoCuenta[i]>2){ // 2 = ultimo estado valido (0 libre, 1 abierta, 2 reservada)
            estadoCuenta[i]=0;
        }
        nombreCuenta[i]=leerLineaArchivo(archivoEstado);
        linea=leerLineaArchivo(archivoEstado);
        separarNumeros(linea,numerosLeidos,60);
        for (j=0;j<cantidadProductosMenu;j++){
            productosEnCuenta[i][j]=(int)numerosLeidos[j];
            if (productosEnCuenta[i][j]<0){
                productosEnCuenta[i][j]=0;
            }
        }
        if (estadoCuenta[i]!=0){
            hayActividad=1;
        }
    }
    // cola
    linea=leerLineaArchivo(archivoEstado);
    totalEntradasCola=textoAEntero(linea);
    if (totalEntradasCola<0 || totalEntradasCola>capacidadCola){
        totalEntradasCola=0;
    }
    for (i=0;i<totalEntradasCola;i++){
        linea=leerLineaArchivo(archivoEstado);
        separarNumeros(linea,numerosLeidos,4); // 4 = cuenta, destino, entregada y hora
        colaCuenta[i]=(int)numerosLeidos[0];
        colaDestino[i]=(int)numerosLeidos[1];
        colaEntregada[i]=(int)numerosLeidos[2];
        colaHoraTomada[i]=(time_t)numerosLeidos[3];
        if (colaCuenta[i]<0 || colaCuenta[i]>=cantidadCuentas){
            colaCuenta[i]=0;
        }
        if (colaDestino[i]<1 || colaDestino[i]>2){ // 2 = barra (1 es cocina)
            colaDestino[i]=1;
        }
        linea=leerLineaArchivo(archivoEstado);
        separarNumeros(linea,numerosLeidos,60);
        for (j=0;j<cantidadProductosMenu;j++){
            colaCantidad[i][j]=(int)numerosLeidos[j];
            if (colaCantidad[i][j]<0){
                colaCantidad[i][j]=0;
            }
        }
        hayActividad=1;
    }
    // acumulados (15 numeros en el mismo orden en que los escribe guardarEstado)
    linea=leerLineaArchivo(archivoEstado);
    separarNumeros(linea,numerosLeidos,60);
    totalVentaBrutaCentavos=(int)numerosLeidos[0];
    totalDescuentosCentavos=(int)numerosLeidos[1];
    totalVentaNetaCentavos=(int)numerosLeidos[2];
    totalPropinasCentavos=(int)numerosLeidos[3];
    totalIvaIncluidoCentavos=(int)numerosLeidos[4];
    totalCobradoCentavos=(int)numerosLeidos[5];
    cantidadCuentasCobradas=(int)numerosLeidos[6];
    pagosEfectivoCantidad=(int)numerosLeidos[7];
    pagosEfectivoMontoCentavos=(int)numerosLeidos[8];
    pagosTarjetaCantidad=(int)numerosLeidos[9];
    pagosTarjetaMontoCentavos=(int)numerosLeidos[10];
    totalRetirosCentavos=(int)numerosLeidos[11];
    totalGastosCentavos=(int)numerosLeidos[12];
    productosAnuladosCantidad=(int)numerosLeidos[13];
    productosAnuladosMontoCentavos=(int)numerosLeidos[14];
    linea=leerLineaArchivo(archivoEstado);
    separarNumeros(linea,numerosLeidos,60);
    for (j=0;j<cantidadProductosMenu;j++){
        unidadesVendidasHoy[j]=(int)numerosLeidos[j];
    }
    // retiros y gastos
    linea=leerLineaArchivo(archivoEstado);
    cantidadMovimientosCaja=textoAEntero(linea);
    if (cantidadMovimientosCaja<0 || cantidadMovimientosCaja>50){ // 50 = maximo de movimientos al dia
        cantidadMovimientosCaja=0;
    }
    for (i=0;i<cantidadMovimientosCaja;i++){
        linea=leerLineaArchivo(archivoEstado);
        separarNumeros(linea,numerosLeidos,2); // 2 = tipo y monto
        movimientoTipo[i]=(int)numerosLeidos[0];
        movimientoMontoCentavos[i]=(int)numerosLeidos[1];
        movimientoDescripcion[i]=leerLineaArchivo(archivoEstado);
        movimientoUsuario[i]=leerLineaArchivo(archivoEstado);
        movimientoHora[i]=leerLineaArchivo(archivoEstado);
    }
    // anulaciones
    linea=leerLineaArchivo(archivoEstado);
    cantidadRegistrosAnulacion=textoAEntero(linea);
    if (cantidadRegistrosAnulacion<0 || cantidadRegistrosAnulacion>50){ // 50 = maximo de anulaciones al dia
        cantidadRegistrosAnulacion=0;
    }
    for (i=0;i<cantidadRegistrosAnulacion;i++){
        registroAnulacion[i]=leerLineaArchivo(archivoEstado);
    }
    archivoEstado.close();
    if (cajaAbierta==1){
        hayActividad=1;
    }
    if (hayActividad==1){
        cout<<"======================================================="<<endl;
        cout<<" se recupero el estado del dia que quedo guardado"<<endl;
        cout<<" (el programa se habia cerrado sin finalizar el dia)"<<endl;
        cout<<"======================================================="<<endl;
        system("pause");
    }
}

// =====================================================================
// PROGRAMA PRINCIPAL
// =====================================================================

// funcion main: carga los archivos guardados y repite: login -> menu principal -> acciones,
// hasta que se finaliza el dia. Despues de cada accion guarda el estado del dia
int main(){
    int opcion;
    cargarMenu();
    cargarContadorCuentas();
    cargarEstado();
    while (diaFinalizado==0){
        iniciarSesion(); // pide usuario y contrasena (si se escribe 0 como usuario, termina el programa)
        while (sesionAbierta==1 && diaFinalizado==0){
            system("cls");
            mostrarEncabezado();
            mostrarOpcionesMenuPrincipal();
            opcion=leerEntero("seleccione una opcion: ",0,8); // 8 = ultima opcion del menu principal
            system("cls");
            switch (opcion){
                case 1:
                    menuMesas();
                    break;
                case 2:
                    tomarOrden();
                    break;
                case 3:
                    anularProducto();
                    break;
                case 4:
                    if (tienePermiso(2)){ // 2 = hasta cajero
                        generarPrecuentaYCobrar();
                    }
                    break;
                case 5:
                    menuCola();
                    break;
                case 6:
                    if (tienePermiso(2)){ // 2 = hasta cajero
                        menuCaja();
                    }
                    break;
                case 7:
                    if (tienePermiso(1)){ // 1 = solo administrador
                        administrarMenu();
                    }
                    break;
                case 8:
                    if (tienePermiso(2)){ // 2 = hasta cajero
                        finalizarDia();
                    }
                    break;
                case 0:
                    sesionAbierta=0;
                    usuarioActual=-1;
                    rolActual=0;
                    cout<<"sesion cerrada"<<endl;
                    break;
            }
            if (diaFinalizado==0){
                guardarEstado(); // despues de cada accion se guarda lo que paso
            }
            if (opcion!=0){
                system("pause");
            }
        }
    }
    cout<<"fin del programa"<<endl;
    return 0;
}