#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <numeric>

//definindo a função logística
double f(double u) {
    return u / (1.0 + std::abs(u));
}

//definindo o modelo KTz logístico
void KTz_model(double& x, double& y, double& z, double k, double T, double H, double I, double l, double d, double x_R) {
    double x_next = f((x - k * y + z + H + I) / T);
    y = x;
    z = (1.0 - d) * z - l * (x - x_R);
    x = x_next;
}

//função para calcular ISI considerando spikes quando x está ficando positivo
std::vector<double> calculate_isi(const std::vector<double>& time, const std::vector<double>& potential) {
    std::vector<double> crossings;
    for (size_t i = 1; i < potential.size(); ++i) {
        if (potential[i-1] <= 0 && potential[i] > 0) {
            crossings.push_back(time[i]);
        }
    }
    std::vector<double> isis;
    for (size_t i = 1; i < crossings.size(); ++i) {
        isis.push_back(crossings[i] - crossings[i-1]);
    }
    return isis;
}

//função para simular o modelo KTz logístico com um valor específico de T
std::vector<double> simulate_model(double T_value, int n = 1000) {
    //! condições iniciais:
    double x_0 = 1.0;
    double y_0 = 1.0;
    double z_0 = 1.0;
    //! parâmetros:
    double k = 0.6;
    double H = 0.0;
    double I = 0.0;
    double l = 0.001;
    double d = 0.001;
    double x_R = -0.2;

    //vetores para armazenar os resultados
    std::vector<double> x_values(n), y_values(n), z_values(n);
    double x_inicial = x_0, y_inicial = y_0, z_inicial = z_0;
    for (int i = 0; i < n; ++i) {
        x_values[i] = x_inicial;
        y_values[i] = y_inicial;
        z_values[i] = z_inicial;
        KTz_model(x_inicial, y_inicial, z_inicial, k, T_value, H, I, l, d, x_R);
    }

    //calcular ISI considerando spikes quando x está ficando positivo
    std::vector<double> time(n);
    std::iota(time.begin(), time.end(), 0);  //preenche o vetor time com valores de 0 a n-1
    return calculate_isi(time, x_values);
}

int main() {

    //valores de T para variar
    std::vector<double> T_values(1000);
    for (size_t i = 0; i < T_values.size(); ++i) {
        T_values[i] = 0.23 + i * (0.26 - 0.23) / (1000 - 1);
    }

    //lista para armazenar os ISIs médios para cada valor de T
    std::vector<double> isis_means;

    //simular o modelo para cada valor de T e calcular o ISI médio
    std::cout << "Iniciando a simulação..." << std::endl;
    for (size_t i = 0; i < T_values.size(); ++i) {
        if (i % 100 == 0) {  // Mostrar progresso a cada 100 iterações
            std::cout << "Simulando para T = " << T_values[i] << " (" << i + 1 << "/" << T_values.size() << ")" << std::endl;
        }
        std::vector<double> isis = simulate_model(T_values[i]);
        if (!isis.empty()) {
            double mean_isi = std::accumulate(isis.begin(), isis.end(), 0.0) / isis.size();
            isis_means.push_back(mean_isi);
        } else {
            isis_means.push_back(0);
        }
    }
    
    //salvar os resultados em um arquivo
    std::cout << "Salvando resultados em 'ISIVariandoT.dat'..." << std::endl;
    std::ofstream outfile("ISIVariandoT.dat");
    for (size_t i = 0; i < T_values.size(); ++i) {
        outfile << T_values[i] << " " << isis_means[i] << std::endl;
    }
    outfile.close();
    std::cout << "Resultados salvos em 'ISIVariandoT.dat'. Use o Gnuplot para visualizar os dados." << std::endl;
    return 0;
}
