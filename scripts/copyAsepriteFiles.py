import glob
import os
import shutil

def copy_aseprite_files(src_folder, dest_folder, dest_folder_2):
    # Certifique-se de que a pasta de destino existe
    os.makedirs(dest_folder, exist_ok=True)

    # Copiar para dest_folder
    for p in glob.glob('**/*.png', recursive=True, root_dir=src_folder):
        if os.path.isfile(os.path.join(src_folder, p)):
            os.makedirs(os.path.join(dest_folder, os.path.dirname(p)), exist_ok=True)
            shutil.copy(os.path.join(src_folder, p), os.path.join(dest_folder, p))
            print(f'{p} ---> {dest_folder}/{p}')

    print(f'\n')

    # Copiar para dest_folder_2
    for filename in os.listdir(src_folder):
        if filename.endswith('.png'):
            src_file_path = os.path.join(src_folder, filename)
            dest_file_path_2 = os.path.join(dest_folder_2, filename)

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