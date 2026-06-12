#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <vector>

// Estructura para representar un cuerpo en el universo
struct Body {
    double mass;
    double radio;
    double x, y, z;
    double vx, vy, vz;
};

// Función para generar el archivo de trabajo en formato tsv
void generateJobFile(int numJobs, const std::string& jobFilename) {
  std::ofstream jobFile(jobFilename);
  
  if (!jobFile.is_open()) {
    std::cerr << "Error abriendo el archivo " << jobFilename << std::endl;
    return;
  }
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distBodies(90000, 100000); // Cuerpos entre 90,000 y 100,000
  std::uniform_int_distribution<int> distTime(100, 10000);
  
  for (int i = 0; i < numJobs; ++i) {
      int numBodies = distBodies(gen);
      int time = distTime(gen);
      jobFile << "univ" << std::setw(3) << std::setfill('0') << i + 1 << ".bin"
          << " " << numBodies << " " << time << std::endl;
  }
  
  jobFile.close();
}

// Función para generar el archivo binario del universo
void generateUniverseFile(int numBodies, const std::string& universeFilename) {
  std::ofstream universeFile(universeFilename, std::ios::binary);
  
  if (!universeFile.is_open()) {
    std::cerr << "Error abriendo el archivo " << universeFilename << std::endl;
    return;
  }
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> massDist(1.0, 10000.0); // Masa [1, 10,000] unidades
  std::uniform_real_distribution<double> radioDist(1.0, 1000.0); // Radio [1, 1,000] unidades
  std::uniform_real_distribution<double> posDist(-1000000.0, 1000000.0); // distancia [-1000000, 1000000]
  std::uniform_real_distribution<double> velDist(-100.0, 100.0); // Velocidad en el rango [-100, 100]

  // Crear un vector de cuerpos y escribirlo en binario
  for (int i = 0; i < numBodies; ++i) {
    Body body;
    body.mass = massDist(gen);
    body.radio = radioDist(gen);
    body.x = posDist(gen);
    body.y = posDist(gen);
    body.z = posDist(gen);
    body.vx = velDist(gen);
    body.vy = velDist(gen);
    body.vz = velDist(gen);
    
    universeFile.write(reinterpret_cast<char*>(&body), sizeof(Body));
  }

  universeFile.close();
}

int main() {
  int numJobs = 1;

  for (int i = 0; i < numJobs; ++i) {
    std::string universeFilename = "univ00" + std::to_string(i + 1) + ".bin";
    int numBodies = rand() % (50001 - 40000) + 40000;
    generateUniverseFile(numBodies, universeFilename);
  }

  std::cout << "Archivos generados correctamente." << std::endl;

  return 0;
}
