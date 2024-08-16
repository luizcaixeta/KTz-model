import numpy as np
import pandas as pd
from tqdm import tqdm
import io
import datashader as ds
import datashader.transfer_functions as tf
import colorcet

#caminho para o arquivo de dados
file_path = r'C:\Users\marcelo\Desktop\caramao\resultados e plots\testeee.dat'

#tamanho do batch
chunk_size = 10**6  # Por exemplo, 1 milhão de linhas por batch

#função para processar o arquivo em batches
def process_file_in_batches(file_path, chunk_size):
    processed_data = []
    total_lines = sum(1 for line in open(file_path, 'r')) - 24  #excluindo linhas de cabeçalho

    with open(file_path, 'r') as file:
        #pular cabeçalho
        for _ in range(24):
            next(file)
        
        #ler e processar em batches
        batch = []
        for line in tqdm(file, total=total_lines, desc="Processando batches"):
            #processar a linha
            line = line.replace('D', 'E').replace('infinity', '1000')
            batch.append(line)

            #se o batch atingir o tamanho definido, converter para DataFrame
            if len(batch) == chunk_size:
                chunk_df = pd.read_csv(io.StringIO(''.join(batch)), delim_whitespace=True, header=None)
                processed_data.append(chunk_df)
                batch = []

        #processar as linhas restantes no último batch
        if batch:
            chunk_df = pd.read_csv(io.StringIO(''.join(batch)), delim_whitespace=True, header=None)
            processed_data.append(chunk_df)

    #concatenar todos os chunks processados
    processed_data = pd.concat(processed_data, ignore_index=True)
    return processed_data

#processar o arquivo em batches
data = process_file_in_batches(file_path, chunk_size)

#separar as colunas
data.columns = ['x_R', 'T', 'col3', 'col4', 'ISIperiod']
x_R = data['x_R'].values
T = data['T'].values
ISIperiod = data['ISIperiod'].values  # Ajuste o índice conforme a posição correta da coluna ISIperiod nos seus dados

#criar o gráfico de dispersão usando datashader
canvas = ds.Canvas(plot_width=800, plot_height=600)
agg = canvas.points(data, 'T', 'x_R', ds.mean('ISIperiod'))
img = tf.shade(agg, cmap=colorcet.fire, how='log')

#salvar o gráfico em um arquivo com 800 dpi
img.to_pil().save('segunda.png', dpi=(800, 800))

print("Gráfico salvo com sucesso em 'segunda.png'")
