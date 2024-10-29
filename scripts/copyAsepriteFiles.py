import os
import shutil

def copy_aseprite_files(src_folder, dest_folder, dest_folder_2):
    # Certifique-se de que a pasta de destino existe
    os.makedirs(dest_folder, exist_ok=True)

    # Percorrer todos os arquivos na pasta de origem
    for filename in os.listdir(src_folder):
        if filename.endswith('.png'):
            src_file_path = os.path.join(src_folder, filename)
            dest_file_path = os.path.join(dest_folder, filename)
            dest_file_path_2 = os.path.join(dest_folder_2, filename)

            # Copiar o arquivo png para a pasta de destino
            shutil.copy(src_file_path, dest_file_path)
            print(f'Arquivo {filename} copiado para: {dest_file_path}')

            shutil.copy(src_file_path, dest_file_path_2)
            print(f'Arquivo {filename} copiado para: {dest_file_path_2}')

# Defina os caminhos das pastas de origem e destino
src_folder = '../submodules/aseprite'
dest_folder = '../Debug/res/Assets/Textures'
dest_folder_2 = '../tools/FrameBoy/res/frames'

# Chamar a função
copy_aseprite_files(src_folder, dest_folder, dest_folder_2)

# Aguardar entrada do usuário para terminar o programa
input("Pressione Enter para terminar o programa...")