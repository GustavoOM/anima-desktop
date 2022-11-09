import threading
import os


def inicia_programa(nome_arquivo):
    os.system('python3 {}'.format(nome_arquivo))

if __name__ == "__main__":

    arquivos = ['servidor.py','cliente.py']

    for arquivo in arquivos:
        inicia_programa(arquivo)