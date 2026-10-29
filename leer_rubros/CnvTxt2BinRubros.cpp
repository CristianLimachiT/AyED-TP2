// CnvTxt2BinRubros.cpp
// HAC

#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

#define record struct
typedef char str5 [ 6],
             str30[31];

record sRub {
  str5  codRub;
  str30 descrip;
};

bool LeerRubros(ifstream &Rub, sRub &rRub) {
  Rub.get(rRub.codRub,6);
  Rub.ignore();
  Rub.get(rRub.descrip,31);
  Rub.ignore();
  return Rub.good();
} // LeerRubros

main () {
  ifstream RubrosTxt("Rubros.Txt");
  fstream Rubros("Rubros.Dat", ios::binary | ios::in | ios::out);
  sRub rRub;

  while (LeerRubros(RubrosTxt,rRub))
    Rubros.write((char *) &rRub, sizeof rRub);
  // Comprobar si se guardo correctamente.
  Rubros.seekg(0);
  while (Rubros.read((char *) &rRub, sizeof rRub))
    cout << right << setw(5) << rRub.codRub << ' '
         << left << setw(30) << rRub.descrip << endl;
  RubrosTxt.close();
  Rubros.close();
  return 0;
}
