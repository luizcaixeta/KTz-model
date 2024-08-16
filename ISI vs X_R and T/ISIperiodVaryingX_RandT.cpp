#include <iostream> //para entrada e saída de dados
#include <fstream> //para manipulação de arquivos
#include <cmath> //para trabalhar com fórmulas matemáticas
#include <vector> //para criar vetores 
#include <map> //para mapear ISIs para suas frequências

//função logística
double logistic_function(double u) {
    return u / (1. + std::abs(u));
}

//função do modelo KTz_log
void KTz_log(double& x, double& y, double& z, double k, double T, double H, double I, double l, double d, double x_R, double& x_next, double& y_next, double& z_next) {
    x_next = logistic_function((x - k * y + z + H + I) / (T));
    y_next = x;
    z_next = (1. - d) * z - l * (x - x_R);
    x = x_next;
    y = y_next;
    z = z_next;    
}

/*
/ x --> potencial de membrana (em unidades arbitrárias) da célula no momento t
/x_next --> potencial da membrana no instante seguinte (t+1)
/y --> variável de recuperação no instante t
/y_next --> variável de recuperação no instante seguinte (t+1)
/z --> canal de corrente iônica lenta no instante t que pode gerar rajadas e picos cardíacos
/z_next --> canal de corrente iônica lenta no instante seguinte (t+1)
/I --> potencial arbitrário gerado por correntes externas devido a sinapses ou eletrodos
/K, T --> parâmetros de controle da dinâmica rápida
/delta --> parâmetro de controle que ajusta o tempo de recuperação de z e controla o período refratário
/labda, x_R --> ajustam a dinâmica lenta de pico e explosão. Particularmente, labda controla o amortecimento das oscilações, enquanto x_R controla a duração do 
/               estouro.
/H --> polariza o potencial da membrana
*/

int main() {
    //! parâmetros:
    double k = 0.6;
    double H = 0.0;
    double I = 0.0;
    double l = 0.001;
    double d = 0.001;
    //! intervalos de variação para x_R e T
    int num_points = 10000;
    std::vector<double> x_R_values(num_points); //cria um vetor de valores de x_R
    std::vector<double> T_values(num_points); //cria um vetor de valores de T
    //! intervalo de valores de x_R
    double x_R_start = -0.199;
    double x_R_end = -0.191;
    //! intervalo de valores de T
    double T_start = 0.234;
    double T_end = 0.255;

    //preenche os vetores de valores x_R e T
    for (int i = 0; i < num_points; ++i) {
        x_R_values[i] = x_R_start + i * (x_R_end - x_R_start) / (num_points - 1);
        T_values[i] = T_start + i * (T_end - T_start) / (num_points - 1);
    }   

    //cria o arquivo de saída "caramao.dat"
    std::ofstream outfile("caramao.dat");
    outfile << "x_R T ISI Frequency" << std::endl;

    //contador de progresso no terminal
    int progresso = 0;
    int total_iterations = num_points * num_points;

    //loop sobre os valores de x_R e T
    for (double x_R : x_R_values) {
        for (double T : T_values) {
            //! condição inicial de x, y e z
            double x = 1.0;
            double y = 1.0;
            double z = 1.0;
            //retorna
            double x_next;
            double y_next;
            double z_next;

            //cria um vetor para salvar o passo em que o spike acontece
            std::vector<int> spike_times;
            int passo = 0;
            int passo_maximo = 50; //número de passos a serem executados
            double x_anterior = x;

            //loop de integração
            while (passo < passo_maximo) {
                KTz_log(x, y, z, k, T, H, I, l, d, x_R, x_next, y_next, z_next);
                passo++;
                //verifica a mudança de sinal no potencial x
                if (x_anterior < x && x_anterior * x < 0) {
                    spike_times.push_back(passo); // .push_back adiciona o elemento ao final do vetor spike_times
                }
                x_anterior = x;
            }

            //calcula o ISI e armazena a frequência
            if (spike_times.size() > 1) {
                std::map<int, int> isi_frequency; // Mapeia ISI para a sua frequência
                for (size_t i = 1; i < spike_times.size(); ++i) {
                    int ISI_value = spike_times[i] - spike_times[i - 1];
                    isi_frequency[ISI_value]++;
                }

                //salva os resultados
                for (const auto& pair : isi_frequency) {
                    int ISI_value = pair.first;
                    int frequency = pair.second;
                    outfile << x_R << " " << T << " " << ISI_value << " " << frequency << std::endl;
                }
            }
            
            //atualizar e printa o progresso no terminal
            progresso++;
            std::cout << "Progress: " << (100.0 * progresso / total_iterations) << "%\r";
            std::cout.flush();
        }
    }
    outfile.close();
    std::cout << "\nResultados salvos em caramao.dat" << std::endl;
    return 0;
}
//? para rodar o ISI por x_R e T usando o gnuplot no terminal:
//! gnuplot -persist -e "set terminal wxt size 800,600; set xlabel 'T'; set ylabel 'x_R'; set zlabel 'ISI'; set grid; splot 'C:/Users/luizf/Desktop/C e C++/projetos C++/testezinho do ISI medio/simulaKTZ_logVariandoT.dat' using 2:1:3 with points palette pointsize 1 pointtype 7"
