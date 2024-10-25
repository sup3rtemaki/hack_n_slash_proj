import os
import shutil

def copy_frameboy_files(src_folder, dest_folder):
    # Certifique-se de que a pasta de destino existe
    os.makedirs(dest_folder, exist_ok=True)

    # Percorrer todos os arquivos na pasta de origem
    for filename in os.listdir(src_folder):
        if filename.endswith('.fdset'):
            src_file_path = os.path.join(src_folder, filename)
            dest_file_path = os.path.join(dest_folder, filename)

            # Copiar o arquivo fdset para a pasta de destino
            shutil.copy(src_file_path, dest_file_path)
            print(f'Arquivo copiado: {filename}')

        if filename.endswith('.png'):
            src_file_path = os.path.join(src_folder, filename)
            dest_file_path = os.path.join(dest_folder, filename)

            # Copiar o arquivo png para a pasta de destino
            shutil.copy(src_file_path, dest_file_path)
            print(f'Arquivo copiado: {filename}')

# Defina os caminhos das pastas de origem e destino
src_folder = '../tools/FrameBoy/res/frames'
dest_folder = '../Debug/res/Assets/Animations'

# Chamar a função
copy_frameboy_files(src_folder, dest_folder)

# Aguardar entrada do usuário para terminar o programa
input("Pressione Enter para terminar o programa...")
