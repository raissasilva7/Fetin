import os
import firebase_admin
from firebase_admin import credentials, firestore
from datetime import datetime
import pytz
import logging
import time

# Suprimindo logs do gRPC usando variável de ambiente
os.environ['GRPC_VERBOSITY'] = 'ERROR'

# Suprimindo mensagens de log do Firebase e gRPC
logging.getLogger('google.auth').setLevel(logging.ERROR)
logging.getLogger('google.auth.transport').setLevel(logging.ERROR)
logging.getLogger('googleapiclient.discovery').setLevel(logging.ERROR)
logging.getLogger('firebase_admin').setLevel(logging.ERROR)

# Caminho para o arquivo de credenciais
service_account_path = r"C:\Users\raiss\Downloads\carereminder-10bab-firebase-adminsdk-eba7k-ad5d86e9b3.json"

# Verifique se o arquivo existe
if not os.path.exists(service_account_path):
    raise FileNotFoundError(f"O arquivo de credenciais não foi encontrado no caminho: {service_account_path}")

# Inicializando o Firebase
cred = credentials.Certificate(service_account_path)
firebase_admin.initialize_app(cred)

# Inicializando o Firestore
db = firestore.client()

# Função para obter a hora e a data atual no fuso horário de São Paulo
def get_data_hora_atual():
    timezone = pytz.timezone('America/Sao_Paulo')
    agora = datetime.now(timezone)
    return agora.strftime('%d/%m/%Y'), agora.strftime('%H:%M')

# Função para verificar os remédios programados e comparar com a hora atual
def verificar_remedios():
    data_atual, hora_atual = get_data_hora_atual()
    print(f"Data atual: {data_atual}")
    print(f"Hora atual: {hora_atual}")
    
    # Obtendo os dados dos remédios do Firestore
    docs = db.collection("TabelaRemedios").stream()
    
    remédios_a_tomar = []

    for doc in docs:
        doc_dict = doc.to_dict()
        
        # Extraindo informações do documento
        compartimento = doc_dict.get('compartimento', 'Desconhecido')
        nome = doc_dict.get('nome', 'Desconhecido')
        dia_previsto = doc_dict.get('dia_previsto', '')
        horario_previsto = doc_dict.get('horario_previsto', '')
        
        # Verifica se o dia e horário previsto coincidem com o dia e horário atuais
        if dia_previsto == data_atual and horario_previsto == hora_atual:
            remédios_a_tomar.append(f"{nome} (Compartimento: {compartimento})")
    
    # Mensagem final sobre os remédios a tomar
    if remédios_a_tomar:
        mensagem = f"\nÉ hora de tomar o(s) remédio(s) agora:\n" + "\n".join(remédios_a_tomar)
    else:
        mensagem = "\nNão é hora de tomar nenhum remédio agora."

    print(mensagem)

# Loop para verificar a cada 1 minuto
while True:
    verificar_remedios()
    time.sleep(60)  # Pausa de 1 minuto
