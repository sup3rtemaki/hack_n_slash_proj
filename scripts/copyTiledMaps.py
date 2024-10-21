import os
import shutil

def copy_and_modify_json_files(src_folder, dest_folder):
    # Certifique-se de que a pasta de destino existe
    os.makedirs(dest_folder, exist_ok=True)

    # Percorrer todos os arquivos na pasta de origem
    for filename in os.listdir(src_folder):
        if filename.endswith('.json'):
            src_file_path = os.path.join(src_folder, filename)
            dest_file_path = os.path.join(dest_folder, filename)

            # Copiar o arquivo JSON para a pasta de destino
            shutil.copy(src_file_path, dest_file_path)
            print(f'Arquivo copiado: {filename}')

            # Abrir e modificar o conteúdo do arquivo copiado
            with open(dest_file_path, 'r', encoding='utf-8') as file:
                content = file.read()

            # Substituir a substring ".tsx" por ".json"
            new_content = content.replace('.tsx', '.json')

            # Substituir a substring "..\/Tilesets\/" por "..\\Assets\\Tilesets\\"
            new_content = new_content.replace('..\/Tilesets\/', '..\\Assets\\Tilesets\\')

            # Salvar as alterações de volta no arquivo
            with open(dest_file_path, 'w', encoding='utf-8') as file:
                file.write(new_content)

            if new_content != content:
                print(f'Arquivo modificado: {filename}')

# Defina os caminhos das pastas de origem e destino
src_folder = '../submodules/ant_hero_tiled_project/Maps'
dest_folder = '../Debug/res/Maps'

# Chamar a função
copy_and_modify_json_files(src_folder, dest_folder)

# Aguardar entrada do usuário para terminar o programa
input("Pressione Enter para terminar o programa...")
