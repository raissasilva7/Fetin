import pyrebase
from datetime import datetime
import pytz

# Configuração do Firebase
config = {
    "apiKey": "AIzaSyAgnO1eC27Vov4DDdIW1CVHg6EJDyxPuQ0",
    "authDomain": "basico-4af5a.firebaseapp.com",
    "databaseURL": "https://basico-4af5a-default-rtdb.firebaseio.com",
    "projectId": "basico-4af5a",
    "storageBucket": "basico-4af5a.appspot.com",
    "messagingSenderId": "945610680559",
    "appId": "1:945610680559:web:8ed1e22523f2abad46d31a",
    "measurementId": "G-5GRMC35J7F"
}

# Inicializando o Firebase
firebase = pyrebase.initialize_app(config)
db = firebase.database()

# Função para obter a hora atual no fuso horário de São Paulo
def get_hora_atual():
    timezone = pytz.timezone('America/Sao_Paulo')
    return datetime.now(timezone).strftime('%H:%M')

# Função para verificar os remédios programados e comparar com a hora atual
def verificar_remedios():
    hora_atual = get_hora_atual()
    print(f"Hora atual: {hora_atual}")
    
    # Obtendo os dados dos remédios do Firebase
    remedios = db.child("remedio").get().val()
    
    if remedios:
        print("\nRemédios programados:")
        for id, info in remedios.items():
            print(f"{info['Hora']}: {info['remedio']}")

        # Verificando se a hora atual coincide com algum horário programado
        if hora_atual in [info['Hora'] for info in remedios.values()]:
            for id, info in remedios.items():
                if info['Hora'] == hora_atual:
                    print(f"\nÉ hora de tomar o remédio agora: {info['remedio']} ")
        else:
            print(f"\nNão é hora de tomar nenhum remédio agora.")
    else:
        print("Nenhum remédio programado.")

# Verifica e exibe os remédios programados
verificar_remedios()
