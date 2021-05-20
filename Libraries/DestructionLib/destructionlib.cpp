#include "destructionlib.h"


/*
void get_tumor_limits(double * mat, int xsize, int ysize, int &left, int &right, int &sup, int &inf){
    left = leftBorder(mat, xsize*ysize);
    right = rightBorder(mat, xsize*ysize);
    sup = superiorBorder(mat, xsize, ysize);
    inf = inferiorBorder(mat, xsize, ysize);
}
*/
/*
int leftBorder(double * mat, int size){
    int node = 0;
    while(mat[node]==0){
        node++;
    }
    return node;
};
*/
/*
int rightBorder(double * mat, int size){
    int lastNode;
    for(int node = 0; node<size ; node++){
        if(mat[node] != 0){
            lastNode = node;
        }
    }
    return lastNode;
};
*/
/*
int superiorBorder(double * mat, int xsize, int ysize){
    int xcoordinate=0, ycoordinate=ysize, aux, node = 0, y,result;
    for(int x = 0; x<xsize; x++){
        aux = 0;
        y = 0;
        while((y<ysize)){
            if( (mat[node]!=0) && (ycoordinate>y) && (aux==0) ){
                xcoordinate = x;
                ycoordinate = y;
                aux = 1;
                result = node;
            }
            y++;
            node++;
            
        }
    }
    return result;
};
*/
/*
int inferiorBorder(double * mat, int xsize, int ysize){
    int xcoordinate=0, ycoordinate=0, aux, node = 0, y,result;
    for(int x = 0; x<xsize; x++){
        aux = 0;
        y = 0;
        while((y<ysize)){
            if( (mat[node]!=0) && (ycoordinate<y) && (aux==0) ){
                xcoordinate = x;
                ycoordinate = y;
                aux = 1;
                result = node;
            }
            y++;
            node++;
            
        }
    }
    return result;
}
*/
/*
double * effectorCellPlacement(int xsize, int ysize, double *T){
    double * rectangle;
    int xleft, xright, ysup, yinf, _, n_cells, leftnode, rightnode, supnode, infnode;
    rectangle = new double[xsize*ysize];
    get_tumor_limits(T, xsize, ysize, leftnode, rightnode, supnode, infnode);

    node_to_coordinates(leftnode, xleft, _, xsize, ysize);
    node_to_coordinates(rightnode, xright, _, xsize, ysize);
    node_to_coordinates(supnode, _, ysup, xsize, ysize);
    node_to_coordinates(infnode, _, yinf, xsize, ysize);
    rectangle = get_squeare(xleft, xright, ysup, yinf, xsize, ysize);
    match_matrices(T, rectangle, xsize, ysize);

    n_cells = int(E_PERCENTAGE*cell_counter(rectangle, xsize*ysize));
    // Ahora que tenemos el rectangulo, vamos a crear una funcion
    // para colocar cada celula de forma aleatoria dentro del rectangulo
    random_place_cell(rectangle, n_cells, xsize, ysize);
    return rectangle;
};
*/
/*
double * get_squeare(int xleft, int xright, int ysup, int yinf, int xsize, int ysize){
    int node = 0;
    double * mat;
    mat = new double[xsize*ysize];
    for(int x = 0; x<xsize; x++){
        for(int y = 0; y<ysize; y++){
            if( ((x>=(xleft-1)) && (y>=(ysup-1))) && ((x<=(xright+1)) && (y<=(yinf+2))) ){
                mat[node] = 1;
            }
            else{
                mat[node] = 0;
            }
            node++;
        }
    }
    return mat;
};
*/
/*
void match_matrices(double *T,double *mat, int xsize, int ysize){
    int totalsize = xsize*ysize;
    for(int node = 0; node<totalsize; node++){
        if(T[node]!=0){
            mat[node] = 0;
        }
    }
};
*/
void tumor_lysis(double T[], double E[], int Ecount[], double D[], double H[], int xsize, int ysize){
    int var;
    std::seed_seq seed{static_cast<long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
                        static_cast<long long>(reinterpret_cast<intptr_t>(&var))};
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dice_distribution(1,2);
    std::vector<int> Tneighbours;
    int dice, node = 0;
    for(int x = 0; x < xsize; x++)
    {
        for(int y = 0; y < ysize; y++)
        {
            //if(E[node]==1){
            if(E[node]>=0){
                Tneighbours = get_specific_neighbours(T, node, 1, 0,'>', xsize, ysize);
                if(Tneighbours.size()!=0){
                    lysis(T, E, Ecount, D, H, node, xsize, ysize, generator);
                }
                else{
                    dice = dice_distribution(generator);
                    switch (dice)
                    {
                    case 1:
                        inactivation(T, E, Ecount, H, node, xsize, ysize, generator);
                        break;
                    
                    case 2:
                        Emigration(T, E, Ecount, H, D, node, xsize, ysize, generator);
                        break;
                    }
                }
                
            }
            node++;
        }
    }
};

void lysis(double T[], double E[], int Ecount[], double D[], double H[], int node, int xsize, int ysize, std::mt19937 generator){
    std::vector<int> Eneighbours, Tneighbours, Hneighbours;
    std::normal_distribution<double> distribution(0,1);
    std::uniform_int_distribution<> u_distrib(1,50);
    int neignode, index;
    double rnd_n = distribution(generator), P;
    
    Eneighbours = get_specific_neighbours(E, node, NEIGBOUR_NUMBER1, 0, '>', xsize, ysize);
    P = 1-exp(-pow(Eneighbours.size()/LYS,2));// PROBAR A PONER A 1x
    if(P>fabs(rnd_n)){
        Tneighbours = get_specific_neighbours(T, node, 1, 0, '>', xsize, ysize);
        index = u_distrib(generator) % Tneighbours.size();
        neignode = Tneighbours[index];
        T[neignode]--; D[neignode]++; Ecount[node]++;

        Hneighbours = get_specific_neighbours(H, node, 1, 0, '>', xsize, ysize);
        for(int j = 0; j<Hneighbours.size(); j++){
            recruitment(T, E, D, H, Hneighbours[j], xsize, ysize, generator);
        }

        if(Ecount[node] == 3){
            E[node] = 0; H[node] = 1; Ecount[node] = 0;
        }
    }
};
void recruitment(double T[], double E[], double D[], double H[], int node, int xsize, int ysize, std::mt19937 generator){
    std::normal_distribution<double> distribution(0,1);
    std::uniform_int_distribution<> u_distrib(1,50);
    std::vector<int> Tneighbours, Hneighbours;
    int index, neignode;
    double rnd_n = distribution(generator), P;
    
    Tneighbours = get_specific_neighbours(T, node, NEIGBOUR_NUMBER1, 0, '>', xsize, ysize);
    P = exp(-1/pow((summation(T, Tneighbours)*REC),2));
    Hneighbours = get_specific_neighbours(H, node, 1, 0, '>', xsize, ysize);
    if(P>fabs(rnd_n) && (Hneighbours.size()>0)){
        index = u_distrib(generator) % Hneighbours.size();
        neignode = Hneighbours[index];
        D[neignode] = 0; H[neignode] = 0; E[neignode] = 1;
    }
};
void inactivation(double T[], double E[], int Ecount[], double H[], int node, int xsize, int ysize, std::mt19937 generator){
    std::normal_distribution<double> distribution(0,1); 
    std::vector<int> Tneighbours;
    double rnd_n = distribution(generator), P;
    Tneighbours = get_specific_neighbours(T, node, NEIGBOUR_NUMBER3, 0, '>', xsize, ysize);
    P = 1 - exp(- pow(1/(summation(T, Tneighbours)*INC),2));
    if(P>fabs(rnd_n)){
        E[node] = 0; Ecount[node] = 0;
        H[node] = 1;
    }
};
//REVISAR RECRUITMENT
int summation(double mat[], std::vector<int> neighbours){
    int node, result = 0;
    for(int i; i<neighbours.size(); i++){
        node = neighbours[i];
        result += mat[node];
    }
    return result;
};
void Emigration(double T[], double E[], int Ecount[], double H[], double D[], int node, int xsize, int ysize, std::mt19937 generator){
    std::uniform_int_distribution<int> u_distrib(1,50);
    std::vector<int> neighbours;
    int index, neignode, temp;
    neighbours = get_neighbours(E, node, 1, xsize, ysize);
    index = u_distrib(generator) % neighbours.size();
    neignode = neighbours[index];
    /*if(E[neignode] == 1){
        temp = Ecount[neignode];
        Ecount[neignode] = Ecount[node];
        Ecount[node] = temp;

        temp = E[neignode];
        E[neignode] = E[node]; 
        E[node] = temp;

    else if(H[neignode] == 1){
        std::cout<<"Migracion: "<<std::endl
        E[neignode] = 1; Ecount[neignode] = Ecount[node]; H[neignode] = 0;
        E[node] = 0; Ecount[node] = 0; H[node] = 1;
    }
    */
    if(E[neignode] != 0){
        temp = Ecount[neignode];
        Ecount[neignode] = Ecount[node];
        Ecount[node] = temp;

        temp = E[neignode];
        E[neignode] = E[node]; 
        E[node] = temp;
    }

    else if(H[neignode] != 0){
        E[neignode] = E[node]; E[node] = 0;
        Ecount[neignode] = Ecount[node]; Ecount[node] = 0; 
        H[node] = H[neignode]; H[neignode] = 0;
        
    }
};

bool no_cells(double mat[], int nnode){
    bool result = true;
    for(int node=0; node<nnode; node++){
        if(mat[node]!=0){
            result = false;
            break;
        }
    }
    return result;
}

void random_place_cell(double * mat, int n_cells_to_place, int xsize, int ysize){
    int var;
    int n, node;
    n = cell_counter(mat,xsize*ysize) - n_cells_to_place;
    std::seed_seq seed{static_cast<long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
                        static_cast<long long>(reinterpret_cast<intptr_t>(&var))};
    std::mt19937 generator(seed);
    while(n>0){
        std::uniform_int_distribution<int> dice_distribution(0,xsize*ysize);
        node = dice_distribution(generator);
        if(mat[node]==1){
            mat[node]=0;
            n--;
        }
    }
}

void effectorCellPlacement(int xsize, int ysize, double T[], double E[]){
    std::vector<int> Tneighbours;
    int ECells, i = 1;
    ECells = E_PERCENTAGE*cell_counter(T, xsize*ysize);
    sector(E, 4);
    while(ECells > 0){
        int allocated = 0;
        for(int node = 0; node<NODE_NUM; node++){
            if( (T[node]==0) & (E[node] == -1)){
                Tneighbours = get_specific_neighbours(T, node, i, 0, '>', xsize, ysize);
                if((!Tneighbours.empty()) ){
                    E[node] = 1;
                    ECells--;
                    allocated++;
                }
            }
        }
        i++;
        if(allocated == 0){
            ECells = 0;
        };
    }
    for(int node = 0; node<NODE_NUM; node++){
        if(E[node]==-1){
            E[node] = 0;
        }
    }
    //random_place_cell
}


void sector(double E[], int quadrant){
    switch (quadrant)
    {
    case 1:
        first_quad(E);
        break;
    case 2:
        first_quad(E);
        second_quad(E);
        break;
    case 3:
        first_quad(E);
        second_quad(E);
        third_quad(E);
        break;
    case 4:
        first_quad(E);
        second_quad(E);
        third_quad(E);
        fourth_quad(E);
        break;
    }
};

void first_quad(double E[]){
    int node;
    for(int x = (2*NX-1)/2; x<(2*NX-1);x++){
        for(int y = 0; y<(2*NY-1)/2; y++){
            coordinates_to_node(node, x, y, 2*NX-1, 2*NY-1 );
            E[node] = -1;
        }
    }
};

void second_quad(double E[]){
    int node;
    for(int x = 0; x<(2*NX-1)/2;x++){
        for(int y = 0; y<(2*NY-1)/2; y++){
            coordinates_to_node(node, x, y, 2*NX-1, 2*NY-1 );
            E[node] = -1;
        }
    }
};

void third_quad(double E[]){
    int node;
    for(int x = 0; x<(2*NX-1)/2;x++){
        for(int y = (2*NY-1)/2; y<(2*NY-1); y++){
            coordinates_to_node(node, x, y, 2*NX-1, 2*NY-1 );
            E[node] = -1;
        }
    }
};

void fourth_quad(double E[]){
    int node;
    for(int x = (2*NX-1)/2; x<(2*NX-1);x++){
        for(int y = (2*NY-1)/2; y<(2*NY-1); y++){
            coordinates_to_node(node, x, y, 2*NX-1, 2*NY-1 );
            E[node] = -1;
        }
    }
};