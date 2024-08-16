#include <tuple>
#include <cmath>
#include <iostream>//para entrada e saída de dados
#include <vector>//para criar um vetor
#include <fstream>//para manipulação de arquivos
//definindo a função logística
double logistic_function(double u) {
    return u / (1. + std::abs(u));
}
//função para calcular o próximo estado do modelo
std::tuple<double, double, double> KTz_model(double x, double y, double z, double k, double T, double H, double I, double l, double d, double x_R) {
    double x_next = logistic_function((x - k * y + z + H + I) / T);
    double y_next = x;
    double z_next = (1. - d) * z - l * (x - x_R);
    return std::tuple<double, double, double>(x_next, y_next, z_next);
}
int main() {
    //! condições iniciais
    double x_0 = -0.35;
    double y_0 = -0.1;
    double z_0 = 0.0;
    //! parâmetros do modelo
    double k = 0.6;
    double H = 0.0;
    double I = 0.0;
    double l = 0.001;
    double d = 0.001;
    double x_R = -0.2;
    //! número de passos 
    int n = 1000;
    //! intervalo para o parâmetro T
    double T_min = 0.1;
    double T_max = 0.4;
    int num_T_values = 50; //número de valores de T para variar
    double T_step = (T_max - T_min) / (num_T_values - 1);
    //abre o arquivo de saída
    std::ofstream outfile("ISImedioporT.dat");
    //verifica se o arquivo abriu corretamente
    if (!outfile.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo de saída!" << std::endl;
        return 1;
    }
    //iteração sobre diferentes valores de T
    for (int i = 0; i < num_T_values; ++i) {
        double T = T_min + i * T_step;
        double x = x_0, y = y_0, z = z_0;
        std::vector<int> spikes; //vetor para armazenar tempos de spikes
        //simulação do modelo
        for (int t = 0; t < n; ++t) {
            //calcula o próximo estado
            double x_next, y_next, z_next;
            std::tie(x_next, y_next, z_next) = KTz_model(x, y, z, k, T, H, I, l, d, x_R);
            x = x_next; y = y_next; z = z_next;
            //detecta spike (supondo que x cruza um limiar)
            if (x > 0.8) {
                spikes.push_back(t); //armazena o tempo do spike
                x = -0.1; //reseta x após o spike (evita múltiplas detecções)
            }
        }
        //calcula o ISI médio
        double isi_sum = 0.0;
        for (size_t j = 1; j < spikes.size(); ++j) {
            isi_sum += spikes[j] - spikes[j - 1];
        }
        double isi_mean = (spikes.size() > 1) ? isi_sum / (spikes.size() - 1) : 0.0;
        //escreve o ISI médio para o valor de T atual no arquivo
        outfile << T << " " << isi_mean << std::endl;
        //exibe o ISI médio para o valor de T atual no terminal
        std::cout << "T = " << T << ", ISI médio = " << isi_mean << std::endl;
    }
    //fecha o arquivo na saída
    outfile.close();
    return 0;
}
//? para rodar o ISI por x_R e T usando o gnuplot no terminal:
//! gnuplot -persist -e "set terminal wxt size 800,600; set xlabel 'T'; set ylabel 'ISI'; set grid; plot 'C:/Users/luizf/Desktop/C e C++/projetos C++/ISI/ISI por T/ISIvariandoT.dat' using 1:2 with points palette pointsize 1 pointtype 7"