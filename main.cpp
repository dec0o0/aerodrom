//
//  main.cpp
//  aerodrom
//
//  Created by mac-mac on 5/20/15.
//  Copyright (c) 2015 mac-mac. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#define MARIME_TD 100
using namespace std;

struct paramFIFO{
    char *text;
    paramFIFO *next;
};

paramFIFO* insereaza_param(paramFIFO *rad, char *text){
    paramFIFO * nou = new paramFIFO;
    nou->text = new char[strlen(text) + 1];
    strcpy(nou->text, text);
    nou->next = NULL;
    if (rad != NULL)
        nou->next = rad;
    return nou;
}

void afiseaza_FIFO(paramFIFO *rad){
    paramFIFO * temp = rad;
    while (temp != NULL){
        cout << temp->text << endl;
        temp = temp->next;
    }
    
    // aici o sa fac interpretarea
}

/// FAV //////////////////// LISTA DUBLA INLANTUITA CIRCULARA

struct nodFAV{
    char *idAeronava;
    bool cargo;
    short int nrLocuri;
    double greutateMaxima;
    nodFAV *urm, *prec;
    
    void afiseaza(){
        cout << idAeronava << " " << (cargo == true ? "CARGO" : "PERSONAL") << " " << nrLocuri << " " << greutateMaxima << endl;
    }
};

void insereaza_FAV(nodFAV *&rad, nodFAV *&ultim, char *idA, bool cargo, short int nr, double greu){
    nodFAV *nou = new nodFAV;
    nou->idAeronava = new char[strlen(idA) + 1];
    strcpy(nou->idAeronava, idA);
    nou->cargo = cargo;
    nou->nrLocuri = nr;
    nou->greutateMaxima = greu;
    if (rad == NULL){
        rad = ultim = nou;
        nou->urm = nou;
        nou->prec = nou;
    }
    else if (rad == ultim){
        nou->urm = rad;
        nou->prec = rad;
        rad->urm = nou;
        rad->prec = nou;
        ultim = nou;
    }
    else{
        nou->prec = ultim;
        ultim->urm = nou;
        nou->urm = rad;
        ultim = nou;
        rad->prec = ultim;
    }
}

void afsieaza_FAV(nodFAV *cap, nodFAV *coada)
{
    nodFAV *temp = cap;
    while(temp != NULL)
    {
        temp->afiseaza();
        if (temp == coada)
            temp = NULL;
        else
            temp = temp->urm;
    }
}

void populeaza_FAV_din_fisier(nodFAV *&rad, nodFAV *&ultim){
    ifstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/flaero.txt");
    while (!f.eof()){
        char buff[256], buff1[256];
        int x, y;
        f >> buff;
        f >> buff1;
        f >> x;
        f >> y;
        insereaza_FAV(rad, ultim, buff, strcmp(buff1, "CARGO") == 0, x, y);
        cout << "Am adaugat aeronava : " << buff << " " << buff1 << " " << x << " " << y << endl;
    }
    f.close();
}

void salveaza_FAV_in_fisier(nodFAV *rad, nodFAV *ultim){
    fstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/flaero.txt", ios :: out);
    nodFAV *temp = rad;
    while(temp != NULL)
    {
        f << temp->idAeronava << " ";
        f << (temp->cargo == true ? "CARGO" : "PERSONAL") << " ";
        f << temp->nrLocuri << " ";
        f << temp->nrLocuri << "\n";
        temp = (temp == ultim ? NULL : temp->urm);
    }
    f.close();
}

nodFAV* cauta_FAV(nodFAV *rad, nodFAV *ultim, char *id){
    nodFAV * aux = rad;
    while (aux != NULL){
        if (strcmp(aux->idAeronava, id) == 0)
            return aux;
        aux = (aux == ultim ? NULL : aux->urm);
    }
    return NULL;
}

void sterge_FAV(nodFAV *elem){
    if (elem != NULL){
        elem->urm->prec = elem->prec;
        elem->prec->urm = elem->urm;
        
        delete []elem->idAeronava;
        delete elem;
    }
}

/// RUZ /////////////////////// TABELA DE DISPERSIE

struct nodRUZ_TD{
    nodFAV * aeronava;
    char *dest, *idZbor;
    short int ora, costuriTotaleRuta;
    nodRUZ_TD *urm;
    
    void afiseaza(){
        cout << idZbor << " " << aeronava->idAeronava << " " << dest << " " << ora << "-" << costuriTotaleRuta << endl;
    }
};

nodRUZ_TD** aloca_RUZ_TD(nodRUZ_TD **elem){
    elem = new nodRUZ_TD* [MARIME_TD];
    for (int i = 0 ; i < MARIME_TD; i++) {
        elem[i] = NULL;
    }
    return elem;
}

int hash_RUZ(char *id){
    int ans = 0;
    for (int i = 0; i < strlen(id); i++) {
        ans += ((id[i] << 6) + 551) - id[i];
    }
    return ans % MARIME_TD;
}

void inserare_RUZ_TD(nodRUZ_TD **&elem, nodFAV *rad, nodFAV *ultim, char *zborId, int ora, char *dest, char *idAero){
    if (elem != NULL){
        nodFAV *aeronava = cauta_FAV(rad, ultim, idAero);
        if (aeronava != NULL){
            int poz = hash_RUZ(zborId);
            nodRUZ_TD *nou = new nodRUZ_TD;
            nou->idZbor = new char[strlen(zborId) + 1];
            strcpy(nou->idZbor, zborId);
            nou->aeronava = aeronava;
            nou->ora = ora;
            nou->dest = new char[strlen(dest) + 1];
            strcpy(nou->dest, dest);
            nou->urm = NULL;
            if (elem[poz] == NULL)
                elem[poz] = nou;
            else{
                nodRUZ_TD * aux = elem[poz];
                while (aux->urm != NULL)
                    aux = aux->urm;
                aux->urm = nou;
            }
            cout << "Am adaugat zborul: " << zborId << " " << dest << ":" << ora << endl;
        }
        else
            cout << "ERROR: Adaugarea a esuat, aeronava " << idAero << " nu exista in flota. (inserare_ruz_td)\n";
    }
}


nodRUZ_TD * cautare_zbor_RUZ(nodRUZ_TD **elem, char *idZbor, int ora){
    if (elem != NULL){
        int poz = hash_RUZ(idZbor);
        nodRUZ_TD * aux = elem[poz];
        while (aux != NULL){
            if (strcmp(aux->idZbor, idZbor))
                return aux;
            aux = aux->urm;
        }
    }
    return NULL;
}

void sterge_zbor_RUZ(nodRUZ_TD **&elem, char *idZbor, int ora){
    if (elem != NULL){
        int poz = hash_RUZ(idZbor);
        nodRUZ_TD * aux = elem[poz];
        if (aux != NULL){
            if (strcmp(aux->idZbor, idZbor)){
                elem[poz] = aux->urm;
            }
            else{
                while (aux->urm != NULL && strcmp(aux->urm->idZbor, idZbor) != 0)
                    aux = aux->urm;
                if (strcmp(aux->urm->idZbor, idZbor) == 0){
                    nodRUZ_TD * de_sters = aux->urm;
                    aux->urm = de_sters->urm;
                    delete[] de_sters->dest;
                    delete de_sters;
                }
            }
        }
    }
}

void afis_RUZ(nodRUZ_TD **elem){
    for (int i = 0; i < MARIME_TD; i++){
        nodRUZ_TD * aux = elem[i];
        if (aux){
            cout << i << "- ";
            while (aux != NULL){
                aux->afiseaza();
                aux = aux->urm;
            }
        }
    }
}

void salveaza_RUZ_TD_in_fisier(nodRUZ_TD **elem){
    if (elem != NULL){
        fstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/program.txt", ios::out);
        for (int i = 0; i < MARIME_TD; i++)
            if (elem[i] != NULL){
                nodRUZ_TD *aux = elem[i];
                while (aux != NULL){
                    f << aux->idZbor << " " << aux->aeronava->idAeronava << " " << aux->dest << " " << aux->ora << endl;
                    aux = aux->urm;
                }
            }
        f.close();
    }
}

void citeste_RUZ_TD_din_fisier(nodRUZ_TD **elem, nodFAV* rad, nodFAV *ultim){
    ifstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/program.txt");
    while (!f.eof()){
        char buff[256], buff1[256], buff2[256];
        int ora;
        f >> buff;
        f >> buff1;
        f >> buff2;
        f >> ora;
        inserare_RUZ_TD(elem, rad, ultim, buff, ora, buff2, buff1);
    }
    f.close();
}

/// REP ///////////////////// LISTA DE LISTE

struct nodPasager{
    char *nume, *prenume, *idRezervare;
    int costBilet;
    nodPasager *urm;
    
    void afiseaza(){
        cout << idRezervare << " " << nume << " " << prenume << " " << costBilet << endl;
    }
};

struct nodREP{
    nodRUZ_TD *zbor;
    nodPasager *pasageri;
    nodREP *urm;
};

nodPasager *creeaza_pasager(char *n, char *p, char *id, int cost){
    nodPasager * nou = new nodPasager;
    nou->nume = new char[strlen(n) + 1];
    strcpy(nou->nume, n);
    nou->prenume = new char[strlen(p) + 1];
    strcpy(nou->prenume, p);
    nou->costBilet = cost;
    nou->idRezervare = new char[strlen(id) + 1];
    strcpy(nou->idRezervare, id);
    return nou;
}

nodREP *adauga_REP(nodREP *rad, nodRUZ_TD *zbor){
    nodREP *nou = new nodREP;
    nou->zbor = zbor;
    nou->pasageri = NULL;
    nou->urm = NULL;
    if (rad == NULL)
        return nou;
    else{
        nodREP * aux = rad;
        if (strcmp(rad->zbor->idZbor , zbor->idZbor) == 0)
            return rad;
        else{
            while (aux->urm != NULL){
                if (strcmp(rad->zbor->idZbor , zbor->idZbor) == 0)
                    return rad;
                aux = aux->urm;
            }
            aux->urm = nou;
            return rad;
        }
    }
}

nodREP *populeaza_REP_cu_zborurile_distincte(nodRUZ_TD **elem){
    nodREP* rad = NULL;
    for (int i = 0; i < MARIME_TD; i++){
        nodRUZ_TD *aux = elem[i];
        while (aux != NULL){
            rad = adauga_REP(rad, aux);
            aux = aux->urm;
        }
    }
    return rad;
}

void afisREP(nodREP *prim){
    nodREP *aux = prim;
    while (aux != NULL){
        cout << "Cod zbor: " << aux->zbor->idZbor << endl;
        nodPasager *pasager = aux->pasageri;
        while (pasager != NULL){
            cout << " * ";
            pasager->afiseaza();
            pasager = pasager->urm;
        }
        aux = aux->urm;
    }
}

void adauga_REPasager(nodREP *&prim, char *idZbor, char *idR, char *n, char *p, int cost){
    nodREP *aux = prim;
    while (aux != NULL){
        if (strcmp(aux->zbor->idZbor, idZbor) == 0){
            nodPasager *cap = aux->pasageri;
            nodPasager *nou = creeaza_pasager(n, p, idR, cost);
            if (cap == NULL)
                aux->pasageri = nou;
            else{
                while (cap->urm != NULL)
                    cap = cap->urm;
                cap->urm = nou;
            }
            cout << "Am adaugat pasagerul : ";
            nou->afiseaza();
            break;
        }
        aux = aux->urm;
    }
}

void adauga_REPasageri_din_fisier(nodREP *&prim){
    ifstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/pasageri.txt");
    while (!f.eof()){
        char buff[256], nume[256], prenume[256], idR[246];
        int pret;
        f >> buff;
        f >> idR;
        f >> nume;
        f >> prenume;
        f >> pret;
        adauga_REPasager(prim, buff, idR, nume, prenume, pret);
    }
    f.close();
}

void salveaza_REPasageri_in_fisier(nodREP *prim){
    fstream f("/Users/macbookproritena/Documents/xcode projects/c++/aerodrom/aerodrom/pasageri.txt", ios::out);
    nodREP *aux = prim;
    while (aux!=NULL){
        nodPasager * pasager = aux->pasageri;
        if (pasager != NULL){
            while (pasager != NULL){
                f << aux->zbor->idZbor;
                f << " ";
                f << pasager->idRezervare;
                f << " ";
                f << pasager->nume;
                f << " ";
                f << pasager->prenume;
                f << " ";
                f << pasager->costBilet;
                f << endl;
                pasager = pasager->urm;
            }
        }
        aux = aux->urm;
    }
    f.close();
}

nodPasager * cautare_REPasager(nodREP *prim, char *idR){
    nodREP *cap = prim;
    while (cap){
        nodPasager * pasager = cap->pasageri;
        while (pasager){
            if (strcmp(pasager->idRezervare, idR) == 0)
                return pasager;
            pasager = pasager->urm;
        }
        cap = cap->urm;
    }
    return NULL;
}

bool sterge_REPasager(nodREP *prim, char *idR){
    nodREP *cap = prim;
    while (cap){
        nodPasager * pasager = cap->pasageri;
        while (pasager->urm != NULL && strcmp(pasager->urm->idRezervare, idR) != 0){
            pasager = pasager->urm;
        }
        if (strcmp(pasager->urm->idRezervare, idR) == 0){
            nodPasager * de_sters = pasager->urm;
            pasager->urm = de_sters->urm;
            delete[] de_sters->nume;
            delete[] de_sters->prenume;
            delete[] de_sters->idRezervare;
            delete de_sters;
            return true;
        }
        
        cap = cap->urm;
    }
    return false;
}

/// CAR /////////////////



int main(int argc, const char * argv[]) {
    
    nodFAV *radFAV, *ultimFAV;
    radFAV = ultimFAV = NULL;
    
    populeaza_FAV_din_fisier(radFAV, ultimFAV);
    //afsieaza_FAV(radFAV, ultimFAV);
    cout << endl;
    
    /*
    nodFAV * x= cauta_FAV(radFAV, ultimFAV, "23AA");
    if (x!= NULL) {
        x->afiseaza();
    }
    else
        cout << "nope";
    
    sterge_FAV(cauta_FAV(radFAV, ultimFAV, "23A"));
    afsieaza_FAV(radFAV, ultimFAV);
     */
    
    nodRUZ_TD ** elem = NULL;
    elem = aloca_RUZ_TD(elem);
    
    citeste_RUZ_TD_din_fisier(elem, radFAV, ultimFAV);
    
/*
    //inserare_RUZ_TD(nodRUZ_TD **&elem, nodFAV *rad, nodFAV *ultim, char *zborId, int ora, char *dest, char *idAero)
    inserare_RUZ_TD(elem, radFAV, ultimFAV, "111x", 11, "Mallorca", "23A");
    inserare_RUZ_TD(elem, radFAV, ultimFAV, "11xvv", 12, "Cluj", "23A");
    inserare_RUZ_TD(elem, radFAV, ultimFAV, "vx", 12, "Madrid", "55X");
    inserare_RUZ_TD(elem, radFAV, ultimFAV, "vx12", 10, "Brugges", "23AA");

    */
    
    //afis_RUZ(elem);
    
    //salveaza_RUZ_TD_in_fisier(elem);
    
    cout << endl;
    nodREP * primREP = NULL;
    primREP = populeaza_REP_cu_zborurile_distincte(elem);
    //
    adauga_REPasageri_din_fisier(primREP);
    cout << endl;
    afisREP(primREP);
    
    
    
    /*
     paramFIFO *rad = NULL;
     cout << "Introduceti comanda dorita:\n";
     char buffer[257];
     do{
     cout << ">: ";
     cin.getline(buffer, 256);
     char *cuv;
     cuv = strtok(buffer, " ");
     while (cuv != NULL){
     rad = insereaza_param(rad, cuv);
     cuv = strtok(NULL, " ");
     }
     afiseaza_FIFO(rad);
     }while(strcmp(buffer, "exit") != 0);
     */
    
    return 0;
}
