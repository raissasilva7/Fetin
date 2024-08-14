import os

# Suprimindo logs do gRPC usando variável de ambiente
os.environ['GRPC_VERBOSITY'] = 'ERROR'
# os.environ['GRPC_TRACE'] = 'none'  # Remover ou comentar esta linha

import firebase_admin
from firebase_admin import credentials, firestore
from datetime import datetime
import pytz
import logging

# Suprimindo mensagens de log do Firebase e gRPC
logging.getLogger('google.auth').setLevel(logging.ERROR)
logging.getLogger('google.auth.transport').setLevel(logging.ERROR)
logging.getLogger('googleapiclient.discovery').setLevel(logging.ERROR)
logging.getLogger('firebase_admin').setLevel(logging.ERROR)

# Caminho para o arquivo de credenciais
service_account_path = "C:/Users/raiss/Downloads/test-9266c-firebase-adminsdk-3c3xf-36ab7da59d.json"

# Verifique se o arquivo existe
if not os.path.exists(service_account_path):
    raise FileNotFoundError(f"O arquivo de credenciais não foi encontrado no caminho: {service_account_path}")

# Inicializando o Firebase
cred = credentials.Certificate(service_account_path)
firebase_admin.initialize_app(cred)

# Inicializando o Firestore
db = firestore.client()

# Função para obter a hora atual no fuso horário de São Paulo
def get_hora_atual():
    timezone = pytz.timezone('America/Sao_Paulo')
    return datetime.now(timezone).strftime('%H:%M')

# Função para verificar os remédios programados e comparar com a hora atual
def verificar_remedios():
    hora_atual = get_hora_atual()
    print(f"Hora atual: {hora_atual}")
    
    # Obtendo os dados dos remédios do Firestore
    docs = db.collection("remedios").stream()
    
    if docs:
        print("\nRemédios programados:")
        for doc in docs:
            doc_dict = doc.to_dict()
            hora = doc_dict.get('Hora')
            remedio = doc_dict.get('remedio')
            if hora:
                # Converte o Timestamp para o formato de hora
                hora_formatada = hora.astimezone(pytz.timezone('America/Sao_Paulo')).strftime('%H:%M')
                print(f"{hora_formatada}: {remedio}")

        # Verificando se a hora atual coincide com algum horário programado
        docs = db.collection("remedios").stream()  # Reinicia o iterator dos documentos
        found = False
        for doc in docs:
            doc_dict = doc.to_dict()
            if doc_dict.get('Hora').astimezone(pytz.timezone('America/Sao_Paulo')).strftime('%H:%M') == hora_atual:
                print(f"\nÉ hora de tomar o remédio agora: {doc_dict.get('remedio')}")
                found = True
        
        if not found:
            print(f"\nNão é hora de tomar nenhum remédio agora.")
    else:
        print("Nenhum remédio programado.")

# Verifica e exibe os remédios programados
verificar_remedios()
