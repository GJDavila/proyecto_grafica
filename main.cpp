#include "opencv2/opencv.hpp"
#include "opencv2/opencv.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include<stdlib.h>
#include<stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>

#define X first
#define Y second
#define show(X) cout << #X << " : " << X << endl;
#define PI 3.14159265
using namespace cv;
using namespace std;

//
// ********************************************************************
//ffmpeg -i out.avi -vf "fps=10" frames_jarra/img%d.png
// Compilar : g++-7 main.cpp -o m `pkg-config --cflags --libs opencv` --std=c++11
// Ejecucion : ./m numero_imagenes nombre_carpeta (0 o 1 para ver los contornos)
// ./m 490 frames_buho_HD 0
// ********************************************************************

typedef pair<float, float> point;
typedef vector<point> v_points;
typedef v_points VP;
vector<VP> VImg;
float rad, ang = 0;
//string direccion_points;
int flag_ver_img = 0;
float angulo,mitad;
ofstream imagen3d("salida.asc");



int count_file_from_folder(string direccion)
{
  DIR *dir;
   struct dirent *ent;
   long count =-2;
   string direc="/home/grimaldo/Documentos/computer_science/grafica/proyecto_final/Scanner3D-Grafica-master/"+direccion;

   dir = opendir(direc.c_str());

   while((ent = readdir(dir)))
   {    ++count;
cout<<count<<endl;
   }

   return count;

}
// escribir puntos en un fichero
void print_poits(v_points VP,int num_arch)
{
    int num_points = VP.size();
  //  string ruta = direccion_points+to_string(num_arch)+".txt";
    show(num_points);
  //  ofstream myfile;
  //  myfile.open(ruta);

    for (int i = 0; i < VP.size(); i++)
    {
        pair<float, float> p1;
        // ro = b/sin(theta)
        VP[i].first = (VP[i].first - mitad)/sin(angulo*PI/180);

        float rx = VP[i].first * cos(rad);
        float ry = VP[i].first * sin(rad);

	//	myfile << rx <<" "<< ry <<" "<< VP[i].second <<'\n';
    imagen3d << rx <<" "<< ry <<" "<< VP[i].second <<'\n';

    }

    //myfile.close();
}

void load_images(string ruta, int num_arch)
{
    int threshold = 800;
    Mat img = imread(ruta);
  //  imshow("img", img); //imagen original

    Mat img_gray;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    Mat contornos;
    Canny(img_gray, contornos, threshold, threshold * 2, 5, true);
  //  if(flag_ver_img)
  //      imshow("contornos", contornos);

    //2) obtener puntos de los contornos
    vector<vector<Point> > p_contornos;
    v_points VP;

    findContours(contornos, p_contornos, RETR_LIST, CV_CHAIN_APPROX_NONE); //CV_CHAIN_APPROX_SIMPLE);

    //guardar puntos en un vector de puntos (VP)
    for (int i = 0; i < p_contornos.size(); i++)
    {
        for (int j = 0; j < p_contornos[i].size(); j++)
        {
            float xx = p_contornos[i][j].x;
            float yy = p_contornos[i][j].y;
            VP.push_back(point(xx, yy)); //y:filas x:columnas
        }
    }
    VImg.push_back(VP);
    if(flag_ver_img)
        waitKey(1e3);

    // if (waitKey(1e6) == 27)
    // {
    //     cout << "OK" << endl;
    // }
    // print_poits(num_arch );

}

void crea_dir(string name){
    const int dir_err = mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void convertir_imagenes(string carpeta_frames)
{
  VideoCapture cap("video.mp4");

   // Check if camera opened successfully
   if(!cap.isOpened()){
     cout << "Error opening video stream or file" << endl;
     return ;
   }

    int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height=  cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    VideoWriter video("out.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);


   while(1){

     Mat frame;
     // Capture frame-by-frame
     cap >> frame;

     // If the frame is empty, break immediately
     if (frame.empty())
       break;

    cv::Mat greyMat,salida;
    cv::cvtColor(frame, greyMat, CV_BGR2GRAY);

    threshold(greyMat,salida, 200, 255, THRESH_BINARY);
    cv::cvtColor(salida,frame, CV_GRAY2BGR);

   imshow("salida",frame);

     video.write(frame);
    char c=(char)waitKey(25);
     if(c==27)
       break;
   }

   // When everything done, release the video capture object
   cap.release();
//"/home/grimaldo/Documentos/computer_science/grafica/proyecto_final/Scanner3D-Grafica-master/prueba1/frames_jarra/"
   // Closes all the frames
   destroyAllWindows();

   string comm =  "ffmpeg -i out.avi -vf \"fps=7\" "+carpeta_frames+"/img%d.png";
   system(comm.c_str());
}
int main(int argc, char **argv)
{

    int num_imagenes = 0;
    angulo = 30;
    mitad = 645;
    string nom_carpeta;
    if (argc != 4)
        cout << "Uso: ./m  num_imgs  nom_carp  ver_no-ver\n";
    else{
      //  num_imagenes = atoi(argv[1]);
        nom_carpeta = argv[2];
        flag_ver_img = atoi(argv[3]);
    }

    string unir = nom_carpeta;
    std::string direccion = unir+"/img";
  //  direccion_points = unir+"_puntos/"+unir;

  //  crea_dir(unir+"_p");
      crea_dir(unir);

    convertir_imagenes(unir);
    num_imagenes=count_file_from_folder(unir);
    cout<<"numero de imagenes "<<num_imagenes<<endl<<endl;

    std::stringstream nameImgs;

    // Carga todas la imagenes en png
    for (int  i = 1; i <= num_imagenes ; i++){
          // rad = ang * PI/180;
        nameImgs << direccion << i << ".png";
        cout << "Ruta leida : " << nameImgs.str() << endl;

        load_images(nameImgs.str() ,i);
		nameImgs = std::stringstream();
    }

    // Procesa la imagen y las guarda en archivos
    for (int  i = 1; i < num_imagenes ; i++){
        rad = ang * PI/180;

        cout << i << " : ";
        print_poits(VImg[i] ,i );
        ang = ang + 360.0 /num_imagenes;

    }

}
