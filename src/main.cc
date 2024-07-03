/*

/==========================================|
|   FILE: main.cc                          |
|   VERSION: 1.1.2a                        |
|   AUTHOR: Linuxperoxo                    |
|   COPYRIGHT: (c) 2024 per Linuxperoxo.   |
|==========================================/

|=====================================================================|
| DESCRIÇÂO |                                                         |
|------------                                                         |
|                                                                     |
|   Um gerenciador de pacotes simples, focado no minimalismo e na     |
| simplicidade.                                                       |
|                                                                     |
| OBS:                                                                |
|   Este pequeno projeto foi desenvolvido por pura diversão e como    | 
| uma forma de praticar minha lógica de programação, visando          |
| aprimorar gradualmente minhas habilidades na linguagem C++          |
|=====================================================================|

|=====================================================================|
| CHANGE LOG |                                                        |
|-------------                                                        |
|                                                                     |
|1.1.1a:                                                              |
| * Melhorias no código;                                              |
| * Agora a base dos argumentos principais foram adicionados;         |
| * Novo .h "package_operations.h";                                   |
| * Adicionado suporte a instalação enfileirada de pacotes;           |
|                                                                     |
|1.1.2a:                                                              |
| * Correção de bugs;                                                 |
| * Novo .h "package.h";                                              |
| * Os argumentos foram removidos no momento;                         |
| * Novo arquivo no repositorio "Pkg-infos";                          |
| * Função package_exist foi adaptada;                                |
| * Classe para pacotes foi adicionado para melhor organização e      |
|   Manipulação do pacote;                                            |
| * Nova função adicionada em package_operations;                     |
|                                                                     |
|1.1.2-1a:                                                            |
| * Correção de bugs;                                                 |
| * Função parse_arguments melhorada;                                 |
| * Destrutor da classe Package adicionado;                           |
| * Melhorias na eficiência do código;                                |
| * Função get_infos adicionada                                       |
|=====================================================================|

|=====================================================================|
| TO DOS |                                                            |
|---------                                                            |
| * Adicionar funções na classe Package para manipulação do pacote;   |
| * Primeiros pacotes serão adicionados ao repositorio para testar    |
|   Funções básicas do gerenciador;                                   |
|=====================================================================| 

*/

//==========================================================| LIBS

#include <cstddef>
#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <libconfig.h++>
#include <filesystem>

//==========================================================| MY HEADERS

#include "check.h"
#include "package_operations.h"
#include "package.h"
#include "color.h"
#include "config_file.h"

//==========================================================| CONST VARS

const std::string config_file = "/etc/lala.conf";
const std::string repo_dir = "/var/lalapkg/repo";
const std::string info_file = "Infopkg";
const std::string build_file = "Buildpkg";
const std::string world_dir = "/var/lalapkg/world/";
const std::string installbin_dir = "/tmp/lalapkg/fakeroot";

//==========================================================| PACKAGE VECTOR

std::vector<std::string> packages_vector;

Config_file* conf_file = new Config_file;

//==========================================================| FUNCTIONS

int emerge(std::string pkg){
  Package* newpkg = nullptr;

  try{
    const std::string pkgroot = package_exist(repo_dir, pkg, build_file, info_file);
  
    Package* newpkg = new Package(pkgroot + info_file, pkgroot + build_file);

    const std::string pkgname = newpkg->get_pkgname();
    const std::string pkgversion = newpkg->get_pkgversion();

    newpkg->makepkg(conf_file->source_dir);
    newpkg->installpkg(world_dir, conf_file->source_dir, conf_file->pkg_dir);

    std::filesystem::remove_all(conf_file->source_dir + "/" + pkgname + "-" + pkgversion);

    delete newpkg;
  }

  catch(std::runtime_error &error){
    std::cerr << RED << "ERROR: " << NC << error.what() << std::endl;
    std::cerr << RED << "ERROR: " << pkg << NC << " package failure!" << std::endl;
    delete newpkg;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int main_switch_loop(char& user_arg){
  switch(user_arg){
    case 'e':
      for(const auto& vector : packages_vector){
        emerge(vector);
      }
    break;

    case 'u':
      for(const auto& vector : packages_vector){
        // unmerge
      }
    break;
  }
  return EXIT_FAILURE;
}

//==========================================================| MAIN

int main(int argc, char* argv[]){
  const std::string* check_this_dirs[] = {&conf_file->source_dir, &conf_file->root_dir, &conf_file->pkg_dir, &installbin_dir, &repo_dir, &world_dir}; 
  const size_t size = sizeof(check_this_dirs) / sizeof(check_this_dirs[0]);
  
  const std::string user_name = getenv("USER");

  if(user_name != "root"){
    std::cerr << RED << "ERROR: " << NC << "Are u sudo?" << std::endl;
    return EXIT_FAILURE;
  }
  
  char arg;

  if(load_config(config_file, conf_file) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  
  if(loadenv_var(conf_file->common_flags, conf_file->jobs, installbin_dir) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  
  if(check_dirs(check_this_dirs, repo_dir, size) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  if(check_argument(argv, argc, arg, packages_vector) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  if(main_switch_loop(arg) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

