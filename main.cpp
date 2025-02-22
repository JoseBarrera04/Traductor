/**
 * @brief Programa para traducir intrucciones MIPS a binario
 * @author Jose Barrera Ramos
 * Arquitectura de Computadores
 * MIPS -> Binarios
 */

// Librerias
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <bitset>
#include <algorithm>
#include "tinyfiledialogs.h"

using namespace std;

// Mapas Desordenados
unordered_map<string, vector<string>> instruccionesMips = {
    {"add", {"R", "000000", "100000"}},
    {"addi", {"I", "001000"}},
    {"addiu", {"I", "001001"}},
    {"addu", {"R", "000000", "100001"}},
    {"and", {"R", "000000", "100100"}},
    {"andi", {"I", "001100"}},
    {"beq", {"I", "000100"}},
    {"bne", {"I", "000101"}},
    {"j", {"J", "000010"}},
    {"jal", {"J", "000011"}},
    {"jr", {"R", "000000", "001000"}},
    {"lbu", {"I", "100100"}},
    {"lhu", {"I", "100101"}},
    {"ll", {"I", "110000"}},
    {"lui", {"I", "001111"}},
    {"lw", {"I", "100011"}},
    {"nor", {"R", "000000", "100111"}},
    {"or", {"R", "000000", "100101"}},
    {"ori", {"I", "001101"}},
    {"slt", {"R", "000000", "101010"}},
    {"slti", {"I", "001010"}},
    {"sltiu", {"I", "001011"}},
    {"sltu", {"R", "000000", "101011"}},
    {"sll", {"R", "000000", "000000"}},
    {"srl", {"R", "000000", "000010"}},
    {"sb", {"I", "101000"}},
    {"sc", {"I", "111000"}},
    {"sh", {"I", "101001"}},
    {"sw", {"I", "101011"}},
    {"sub", {"R", "000000", "100010"}},
    {"subu", {"R", "000000", "100011"}},
    {"div", {"R", "000000", "011010"}},
    {"divu", {"R", "000000", "011011"}},
    {"lwc1", {"I", "110001"}},
    {"ldc1", {"I", "110001"}},
    {"mfhi", {"R", "000000", "010000"}},
    {"mflo", {"R", "000000", "010010"}},
    {"mfc0", {"R", "000000", "000000"}},
    {"mult", {"R", "000000", "011000"}},
    {"multu", {"R", "000000", "011001"}},
    {"sra", {"R", "000000", "000011"}},
    {"swcl", {"I", "101011"}},
    {"sdcl", {"I", "101011"}}
};

unordered_map<string, string> registroMIPS = {
    {"$0", "00000"},
    {"$zero", "00000"},
    {"$at", "00001"},
    {"$v0", "00010"},
    {"$v1", "00011"},
    {"$a0", "00100"},
    {"$a1", "00101"},
    {"$a2", "00110"},
    {"$a3", "00111"},
    {"$t0", "01000"},
    {"$t1", "01001"},
    {"$t2", "01010"},
    {"$t3", "01011"},
    {"$t4", "01100"},
    {"$t5", "01101"},
    {"$t6", "01110"},
    {"$t7", "01111"},
    {"$s0", "10000"},
    {"$s1", "10001"},
    {"$s2", "10010"},
    {"$s3", "10011"},
    {"$s4", "10100"},
    {"$s5", "10101"},
    {"$s6", "10110"},
    {"$s7", "10111"},
    {"$t8", "11000"},
    {"$t9", "11001"},
    {"$k0", "11010"},
    {"$k1", "11011"},
    {"$gp", "11100"},
    {"$sp", "11101"},
    {"$fp", "11110"},
    {"$ra", "11111"}
};

// Mapas Etiquetas
unordered_map<string, unsigned int> etiquetas;

// String comandoCompleto
string comandoCompleto;

// Funciones
void abrirArchivo(vector<vector<string>>& listaCompletaComandos, unsigned int& pc);
void limpiarComando(string& linea);
vector<string> separarComandos(string& linea);
void traduccirMipsToBinario(vector<vector<string>>& listaCompletaComandos, unsigned int& pc);
void traducirTipoR(vector<string> lineaMips, unsigned int& pc);
void traducirTipoI(vector<string> lineaMips, unsigned int& pc);
void traducirTipoJ(vector<string> lineaMips, unsigned int& pc);

/**
 * @brief Permite al usuario ingresar instrucciones MIPS línea por línea hasta que se ingrese #
 * @param listaCompletaComandos Vector donde se almacenan las instrucciones por separado
 */
void ingresarInstrucciones(vector<vector<string>>& listaCompletaComandos, unsigned int& pc) {
    string linea;
    cout << "Ingrese las instrucciones MIPS línea por línea. Ingrese # para finalizar." << endl;
    while (true) {
        cout << "> ";
        getline(cin, linea);
        if (linea == "#") {
            break;
        }
        limpiarComando(linea);
        if (!linea.empty()) {
            if (linea.back() == ':') {
                string etiqueta = linea.substr(0, linea.size() - 1);
                etiquetas[etiqueta] = pc + 0x0004;
            } else {
                listaCompletaComandos.push_back(separarComandos(linea));
                pc += 0x0004;
            }
        }
    }
}

/**
 * Main Principal
 * @return Codigo de estado de la ejecucion
 */
int main() {
    unsigned int pc = 0x00000000;
    vector<vector<string>> listaCompletaComandos;
    abrirArchivo(listaCompletaComandos, pc);
    //ingresarInstrucciones(listaCompletaComandos, pc);
    traduccirMipsToBinario(listaCompletaComandos, pc);
    cout << comandoCompleto << endl;
    return 0;
}

/**
 * @brief Abre un archivo (.s || .txt) y carga las instrucciones MIPS en una lista
 * @param listaCompletaComandos Vector donde se almacenan las instrucciones por separado
 */
void abrirArchivo(vector<vector<string>>& listaCompletaComandos, unsigned int& pc) {
    const char *filter[2] = {"*.s", "*.txt"};
    const char *filePath = tinyfd_openFileDialog(
        "Selecciona un archivo", "", 2, filter, "Archivos MIPS (*.s, *.txt)", 0
    );

    if (filePath) {
        cout << "Archivo seleccionado: " << filePath << endl;
        ifstream file(filePath);
        if (file.is_open()) {
            string linea;
            while (getline(file, linea)) {
                limpiarComando(linea);
                if(!linea.empty()) {
                    if(linea.back() == ':') {
                        string etiqueta = linea.substr(0, linea.size()-1);
                        etiquetas[etiqueta] = pc + 0x0004;
                    } else {
                        listaCompletaComandos.push_back(separarComandos(linea));
                        pc += 0x0004;
                    }
                }
            }
            // Este mapa solo sirve para ver etiquetas en el debug
            unordered_map<string, unsigned int> clon = etiquetas;
            file.close();
        } else {
            cout << "No se pudo abrir el archivo." << endl;
        }
    } else {
        cout << "No se selecciono ningun archivo." << endl;
    }
}

/**
 * @brief Limpia una linea de comando eliminando los comentarios
 * @param linea Linea de comando a limpiar
 */
void limpiarComando(string& linea) {
    int posComentarios = linea.find("#");
    if(posComentarios >= 0) {
        linea = linea.substr(0, posComentarios);
    }

    linea.erase(remove(linea.begin(), linea.end(), ','), linea.end());
}

/**
 * @brief Separa una linea de comando en sus componentes
 * @param linea Linea de comando a separar
 * @return Vector con los componenetes del comando
 */
vector<string> separarComandos(string& linea) {
    vector<string> comandos;
    stringstream ss(linea);
    string comando;

    while (ss >> comando) {
        comandos.push_back(comando);
    }

    return comandos;
}

/**
 * @brief Traduce las instrucciones MIPS a binario
 * @param listaCompletaComandos Vector con las instrucciones MIPS
 */
void traduccirMipsToBinario(vector<vector<string>>& listaCompletaComandos, unsigned int& pc) {
    for(vector<vector<string>>::iterator it = listaCompletaComandos.begin(); it != listaCompletaComandos.end(); it++) {
        if(it->empty() == 0) {
            string instruccion = (*it)[0];
            if(instruccionesMips.find(instruccion) != instruccionesMips.end()) {
                vector<string> info = instruccionesMips[instruccion];
                string binario;
                vector<string> lineaMips = (*it);
                if(info[0] == "R") {
                    traducirTipoR(lineaMips, pc);
                } else if(info[0] == "I") {
                    traducirTipoI(lineaMips, pc);
                } else if(info[0] == "J") {
                    traducirTipoJ(lineaMips, pc);
                }

                cout << binario << endl;
            } else {
                cout << "Instruccion no reconocida: " << instruccion << endl;
            }
        }
    }
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo R
 * @param lineaMips
 */
void traducirTipoR(vector<string> lineaMips, unsigned int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    string funct = instruccionesMips[lineaMips[0]][2];
    string rs = "00000", rt = "00000", rd = "00000", shamt = "00000";

    if (lineaMips[0] == "jr") {
        rs = registroMIPS[lineaMips[1]];
    } else if (lineaMips[0] == "sll" || lineaMips[0] == "srl" || lineaMips[0] == "sra") {
        rd = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
        shamt = bitset<5>(stoi(lineaMips[3])).to_string();
    } else if (lineaMips[0] == "mfhi" || lineaMips[0] == "mflo") {
        rd = registroMIPS[lineaMips[1]];
    } else if (lineaMips[0] == "mult" || lineaMips[0] == "div") {
        rs = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
    } else {
        rd = registroMIPS[lineaMips[1]];
        rs = registroMIPS[lineaMips[2]];
        rt = registroMIPS[lineaMips[3]];
    }

    comandoCompleto += opcode + rs + rt + rd + shamt + funct;
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo I
 * @param lineaMips
 */
void traducirTipoI(vector<string> lineaMips, unsigned int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    string rs, rt, immediate;

    if (lineaMips[0] == "lw" || lineaMips[0] == "sw") {
        rt = registroMIPS[lineaMips[1]];
        string offsetReg = lineaMips[2];
        size_t pos = offsetReg.find('(');
        int enteroImmediate = stoi(offsetReg.substr(0, pos));
        rs = registroMIPS[offsetReg.substr(pos + 1, offsetReg.find(')') - pos - 1)];
        immediate = bitset<16>(enteroImmediate).to_string();
    } else if (lineaMips[0] == "beq" || lineaMips[0] == "bne") {
        rs = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
        int offset = (etiquetas[lineaMips[3]] - (pc + 0x0004)) >> 2;
        immediate = bitset<16>(offset).to_string();
    } else if (lineaMips[0] == "lui") {
        rt = registroMIPS[lineaMips[1]];
        rs = "00000";
        immediate = bitset<16>(stoi(lineaMips[2])).to_string();
    } else {
        rt = registroMIPS[lineaMips[1]];
        rs = registroMIPS[lineaMips[2]];
        immediate = bitset<16>(stoi(lineaMips[3])).to_string();
    }

    //cout << "tipo i:" << endl;
    //comandoCompleto += opcode + " " + rs + " " + rt + " " + immediate;
    comandoCompleto += opcode + rs + rt + immediate;
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo J
 * @param lineaMips
 */
void traducirTipoJ(vector<string> lineaMips, unsigned int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    string direccionEtiqueta = lineaMips[1];
    unsigned int direccion = etiquetas[direccionEtiqueta] >> 2;
    string direccionBinaria = bitset<26>(direccion).to_string();

    comandoCompleto += opcode + direccionBinaria;
}