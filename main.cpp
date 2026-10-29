
/*
Programa:       Proceso de archivos de rubros y articulos
Version:        1
Descripcion:    Procesamiento de archivos binarios de rubros y Articulos
Curso:          AyED K1094
Fecha Entrega:  03/11/2026
Turno:          Martes - Turno Noche

Nro. del Grupo:
Integrantes:
  259.495-0    Obrutsky         Sebastian
  259.522-9    Limachi          Cristian
  259.488-2    Anselmo Frette   Victor
  260.367-6    Monrroy          Daniel
  259.262-9    Romero           Santiago
*/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <ctime>

using namespace std;

typedef char str5[6];
typedef char str7[8];
typedef char str30[31];

// Estructura para lectura del archivo binario Articulos.Dat
struct sArticulo {
    str7  codArt;
    str30 descrip;
    short stkAct;
    str5  codRub;
    float preUni;
};

// Estructura para lectura del archivo binario Rubros.Dat
struct sRubro {
    str5  codRub;
    str30 descrip;
};
// Subnodo de Lista de Artículos (Tamaño total: 37 bytes)
typedef struct sNodoArt *tSubListaArt;

// Info del nodo principal de Rubros
struct tInfoRub {
    str5         codRub;
    tSubListaArt pSubLista;
};

// Info del sub-nodo de Artículos
struct tInfoArt {
    str30 descrip;
    long  refArt; // Dirección/offset en bytes dentro de Articulos.Dat
};

// Nodo Principal de Lista de Rubros (Tamaño total: 14 bytes)
typedef struct sNodoRub *tListaRub;

struct sNodoArt {
    tInfoArt    info;
    tSubListaArt sgte;
};

struct sNodoRub {
    tInfoRub  info;
    tListaRub sgte;
};

// Apertura de archivos binarios utilizando flujos de C++ (stream)
void Abrir(ifstream &Rubros, ifstream &Articulos) {
    Rubros.open("Rubros.Dat", ios::binary | ios::in);
    Articulos.open("Articulos.Dat", ios::binary | ios::in);
}

// Cierre de flujos de archivos
void Cerrar(ifstream &Rubros, ifstream &Articulos) {
    Rubros.close();
    Articulos.close();
}

// Agrega un nodo al final de la lista principal de Rubros
void AgregarUltimoNodo(tListaRub &Lista, tListaRub &ListaFin, tInfoRub valor) {
    tListaRub pNvo = new sNodoRub;
    pNvo->info = valor;
    pNvo->sgte = NULL;
    if (!Lista)
        Lista = pNvo;
    else
        ListaFin->sgte = pNvo;
    ListaFin = pNvo;
}

// Lee Rubros.Dat (1er acceso secuencial) y arma la lista principal de Rubros
void ArmarListaRubros(ifstream &Rubros, tListaRub &ListaRubros) {
    sRubro    rRub;
    tInfoRub  rInfoR;
    tListaRub ListaFin = NULL;

    Rubros.read((char *)&rRub, sizeof(sRubro));
    while (!Rubros.eof()) {
        strcpy(rInfoR.codRub, rRub.codRub);
        rInfoR.pSubLista = NULL;
        AgregarUltimoNodo(ListaRubros, ListaFin, rInfoR);
        Rubros.read((char *)&rRub, sizeof(sRubro));
    }
}

// Función condicional inline/operador ternario encapsulado (estilo Cuello)
tListaRub iif(bool exprLog, tListaRub ptr1, tListaRub ptr2) {
    if (exprLog)
        return ptr1;
    else
        return ptr2;
}

// Busca un nodo en la lista ordenada por clave Cód. Rubro
tListaRub BuscarNodoClvOrd(tListaRub &Lista, str5 codRub) {
    tListaRub pAct = Lista;
    while (pAct && strcmp(codRub, pAct->info.codRub) > 0) {
        pAct = pAct->sgte;
    }
    return iif(pAct && strcmp(codRub, pAct->info.codRub) == 0, pAct, NULL);
}

// Inserción al inicio de la sublista de Artículos
void InsertaInicio(tSubListaArt &Lista, tInfoArt valor) {
    tSubListaArt pNvo = new sNodoArt;
    pNvo->info = valor;
    pNvo->sgte = Lista;
    Lista = pNvo;
}

// Inserción a continuación del nodo provisto
void InsertaEnLugar(tSubListaArt &pNodo, tInfoArt valor) {
    tSubListaArt pNvo = new sNodoArt;
    pNvo->info = valor;
    pNvo->sgte = pNodo->sgte;
    pNodo->sgte = pNvo;
}

// Inserción en el medio/final manteniendo orden por Descripción de Artículo
void InsertaEnMedio(tSubListaArt &Lista, tInfoArt valor) {
    tSubListaArt pAct = Lista;
    while (pAct->sgte && strcmp(valor.descrip, pAct->sgte->info.descrip) > 0) {
        pAct = pAct->sgte;
    }
    InsertaEnLugar(pAct, valor);
}

// Módulo despachador de inserción ordenada en sublista
void InsertaNodo(tSubListaArt &Lista, tInfoArt valor) {
    if (!Lista || strcmp(valor.descrip, Lista->info.descrip) < 0)
        InsertaInicio(Lista, valor);
    else
        InsertaEnMedio(Lista, valor);
}

// Lee Articulos.Dat (1er acceso secuencial) y arma las sublistas colgadas
void ArmarSubListaArticulos(ifstream &Articulos, tListaRub &ListaRubros) {
    sArticulo   rArt;
    tInfoArt    rInfoA;
    tListaRub   pRubroEncontrado;
    long        posByte;

    posByte = Articulos.tellg();
    Articulos.read((char *)&rArt, sizeof(sArticulo));

    while (!Articulos.eof()) {
        pRubroEncontrado = BuscarNodoClvOrd(ListaRubros, rArt.codRub);
        if (pRubroEncontrado) {
            strcpy(rInfoA.descrip, rArt.descrip);
            rInfoA.refArt = posByte;
            InsertaNodo(pRubroEncontrado->info.pSubLista, rInfoA);
        }
        posByte = Articulos.tellg();
        Articulos.read((char *)&rArt, sizeof(sArticulo));
    }
}

// Suprime y retorna el primer nodo de la lista de Rubros
void SacarPrimerNodoRub(tListaRub &Lista, tInfoRub &valor) {
    tListaRub pElim = Lista;
    valor = Lista->info;
    Lista = Lista->sgte;
    delete pElim;
}

// Suprime y retorna el primer nodo de la sublista de Artículos
void SacarPrimerNodoArt(tSubListaArt &Lista, tInfoArt &valor) {
    tSubListaArt pElim = Lista;
    valor = Lista->info;
    Lista = Lista->sgte;
    delete pElim;
}

// Genera una cadena repitiendo un carácter n veces
string replicate(char car, short n) {
    string cad = "";
    for (short i = 0; i < n; i++) {
        cad += car;
    }
    return cad;
}

// Obtiene la fecha del sistema en formato YYYYMMDD
// Como se usa un puntero usamos el -> para indicar "."
long GetDate(int &year, int &mes, int &dia, int &ds) {
    time_t t = time(NULL);
    tm *fec = localtime(&t);
    year = fec->tm_year + 1900;
    mes = fec->tm_mon + 1;
    dia = fec->tm_mday;
    ds = fec->tm_wday;
    return year * 10000L + mes * 100L + dia;
}

// Emisión del informe final hacia ListadoRubrosArticulos.Lst
void ListadoRubrosArticulos(ifstream &Rubros, ifstream &Articulos, 
    tListaRub &ListaRubros) 
    {
    sRubro    rRub;
    sArticulo rArt;
    tInfoArt  rInfoA;
    int       year, mes, dia, ds;
    short     cantItemsRub;
    short     maxItems = -1;
    str5      codRubMax = "";

    // Redirección de la salida hacia el archivo de texto
    freopen("ListadoRubrosArticulos.Lst", "w", stdout);

    GetDate(year, mes, dia, ds);

    // Encabezado del reporte
    cout << "Fecha : " << setfill('0') << setw(2) << dia << "-"
         << setw(2) << mes << "-" << setw(4) << year << setfill(' ') << endl;
    cout << setw(60) << "Listado por Rubros y Articulos" << endl;
    cout << replicate('_', 88) << endl << endl;

    // 2do acceso secuencial a Rubros.Dat
    Rubros.clear();
    Rubros.seekg(0, ios::beg);

    Rubros.read((char *)&rRub, sizeof(sRubro));
    while (!Rubros.eof()) {
        cantItemsRub = 0;

        cout << "Cod.Rubro: " << left << setw(5) << rRub.codRub 
             << "                "
             << "Nombre Rubro: " << setw(30) << rRub.descrip << right << endl;
        cout << "Cod.Articulo    Descripcion";
        cout << "                                                    Pre.Unit."
             << endl;

        tListaRub pNodoRub = BuscarNodoClvOrd(ListaRubros, rRub.codRub);

        if (pNodoRub && pNodoRub->info.pSubLista) {
            tSubListaArt pSubAct = pNodoRub->info.pSubLista;
            while (pSubAct) {
                rInfoA = pSubAct->info;

                // Acceso al azar a Articulos.Dat utilizando refArt
                Articulos.clear();
                Articulos.seekg(rInfoA.refArt, ios::beg);
                Articulos.read((char *)&rArt, sizeof(sArticulo));

                cout << left << setw(16) << rArt.codArt
                     << setw(60) << rArt.descrip
                     << "$ " << right << setw(10) << fixed << setprecision(2) 
                     << rArt.preUni << endl;

                cantItemsRub++;
                pSubAct = pSubAct->sgte;
            }
        }

        cout << endl;
        cout << "Cantidad de items: " << setw(4) << cantItemsRub << endl;
        cout << replicate('_', 88) << endl << endl;

        if (cantItemsRub > maxItems) {
            maxItems = cantItemsRub;
            strcpy(codRubMax, rRub.codRub);
        }

        Rubros.read((char *)&rRub, sizeof(sRubro));
    }

    cout << "Codigo de Rubro con mayor cantidad de items: " 
         << codRubMax << endl;

    // Restauración de la consola
    freopen("CON", "w", stdout);
}

int main() {
    ifstream  Rubros;
    ifstream  Articulos;
    tListaRub ListaRubros = NULL;

    Abrir(Rubros, Articulos);
    ArmarListaRubros(Rubros, ListaRubros);
    ArmarSubListaArticulos(Articulos, ListaRubros);
    ListadoRubrosArticulos(Rubros, Articulos, ListaRubros);
    Cerrar(Rubros, Articulos);

    return 0;
}


