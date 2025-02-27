/**
 * @brief Programa para traducir intrucciones MIPS a binario
 * @authors Jose Barrera Ramos y Santiago Guevara I
 * Arquitectura de Computadores
 * MIPS -> Binarios
 * Parcial Practico
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
unordered_map<string, int> etiquetas;

// String comandoCompleto
string comandoCompleto;

// Funciones
void abrirArchivo(string& filePath);
void calcularEtiquetas(string filePath, int& pc);
void limpiarComando(string& linea);
vector<string> separarComandos(string& linea);
void traducirMipsToBinario(string filePath, int& pc);
void traducirTipoR(vector<string> lineaMips, int& pc);
void traducirTipoI(vector<string> lineaMips, int& pc);
void traducirTipoJ(vector<string> lineaMips, int& pc);

/**
 * Main Principal
 * @return Codigo de estado de la ejecucion
 */
int main() {
    int pc = 0x00040000;
    string filePath;
    abrirArchivo(filePath);
    calcularEtiquetas(filePath, pc);
    traducirMipsToBinario(filePath, pc);
    cout << "Comando completo:" << endl << comandoCompleto << endl;
    return 0;
}

/**
 * @brief Abre un archivo (.s || .txt) y carga las instrucciones MIPS en una lista
 */
void abrirArchivo(string& filePath) {
    const char *filter[2] = {"*.s", "*.txt"};
    const char *path = tinyfd_openFileDialog(
        "Selecciona un archivo", "", 2, filter, "Archivos MIPS (*.s, *.txt)", 0
    );

    if (path) {
        cout << "Archivo seleccionado: " << path << endl;
        filePath = string(path);
    } else {
        cout << "No se selecciono ningun archivo." << endl;
        filePath = "";
    }
}

/**
 * 
 * @param filePath
 * @param pc 
 */
void calcularEtiquetas(string filePath, int &pc) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    string linea;
    pc = 0x00000000;
    bool primeraEtiqueta = true;
    while (getline(file, linea)) {
        limpiarComando(linea);
        if (!linea.empty()) {
            if (linea.back() == ':') {
                string etiqueta = linea.substr(0, linea.size() - 1);
                if (primeraEtiqueta) {
                    etiquetas[etiqueta] = pc;
                    primeraEtiqueta = false;
                } else {
                    etiquetas[etiqueta] = pc;
                    //etiquetas[etiqueta] = pc + 0x0004;
                }
            } else {
                pc += 0x0004;
            }
        }
    }

    for (auto& etiqueta : etiquetas) {
        if (etiqueta.second == pc + 0x0004) {
            etiqueta.second = 0;
        }
    }

    unordered_map<string, int> aux = etiquetas;
    file.close();
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
 */
void traducirMipsToBinario(string filePath, int& pc) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
    } else {
        string linea;
        pc = 0x00000000;
        while (getline(file, linea)) {
            limpiarComando(linea);
            if (!linea.empty() && linea.back() != ':') {
                vector<string> lineaMips = separarComandos(linea);
                if (instruccionesMips.find(lineaMips[0]) == instruccionesMips.end()) {
                    cout << "Instruccion no reconocida: " << lineaMips[0] << endl;
                    return;
                }
                vector<string> info = instruccionesMips[lineaMips[0]];
                cout << "actual: " << pc << endl;
                if (info[0] == "R") {
                    traducirTipoR(lineaMips, pc);
                } else if (info[0] == "I") {
                    traducirTipoI(lineaMips, pc);
                } else if (info[0] == "J") {
                    traducirTipoJ(lineaMips, pc);
                }
                cout << endl;
                pc += 0x0004;
            }
        }
        file.close();
    }
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo R
 * @param lineaMips
 */
void traducirTipoR(vector<string> lineaMips, int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    string funct = instruccionesMips[lineaMips[0]][2];
    string rs = "00000", rt = "00000", rd = "00000", shamt = "00000";

    if (lineaMips[1] == "$0" || lineaMips[1] == "$zero" || lineaMips[1] == "$gp" || lineaMips[1] == "$k0" || lineaMips[1] == "$k1") {
        return;
    }

    if (lineaMips[0] == "jr") {
        if (lineaMips.size() != 2 || registroMIPS.find(lineaMips[1]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": 'jr' requiere un registro valido" << endl;
            return;
        }
        rs = registroMIPS[lineaMips[1]];
    } else if (lineaMips[0] == "sll" || lineaMips[0] == "srl" || lineaMips[0] == "sra") {
        if (lineaMips.size() != 4 || registroMIPS.find(lineaMips[1]) == registroMIPS.end() ||
            registroMIPS.find(lineaMips[2]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere rd, rt y shamt validos" << endl;
            return;
        }
        rd = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
        int shamt_val;
        try {
            shamt_val = stoi(lineaMips[3]);
        } catch (...) {
            cout << "Error en PC 0x" << hex << pc << ": 'shamt' debe ser un numero valido" << endl;
            return;
        }
        if (shamt_val < 0 || shamt_val > 31) {
            cout << "Error en PC 0x" << hex << pc << ": 'shamt' debe estar entre 0 y 31" << endl;
            return;
        }
        shamt = bitset<5>(shamt_val).to_string();
    } else if (lineaMips[0] == "mfhi" || lineaMips[0] == "mflo") {
        if (lineaMips.size() != 2 || registroMIPS.find(lineaMips[1]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere un registro valido" << endl;
            return;
        }
        rd = registroMIPS[lineaMips[1]];
    } else if (lineaMips[0] == "mult" || lineaMips[0] == "div") {
        if (lineaMips.size() != 3 || registroMIPS.find(lineaMips[1]) == registroMIPS.end() ||
            registroMIPS.find(lineaMips[2]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere dos registros validos" << endl;
            return;
        }
        rs = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
    } else {
        if (lineaMips.size() != 4 || registroMIPS.find(lineaMips[1]) == registroMIPS.end() ||
            registroMIPS.find(lineaMips[2]) == registroMIPS.end() || registroMIPS.find(lineaMips[3]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere rd, rs y rt validos" << endl;
            return;
        }
        rd = registroMIPS[lineaMips[1]];
        rs = registroMIPS[lineaMips[2]];
        rt = registroMIPS[lineaMips[3]];
    }

    string binario = opcode + rs + rt + rd + shamt + funct;
    cout << "0x" << hex << pc << ": " << binario << endl;
    comandoCompleto += binario;
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo I
 * @param lineaMips
 */
void traducirTipoI(vector<string> lineaMips, int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    string rs, rt, immediate;

    if (lineaMips[1] == "$0" || lineaMips[1] == "$zero" || lineaMips[1] == "$gp" || lineaMips[1] == "$k0" || lineaMips[1] == "$k1") {
        return;
    }

    if (lineaMips[0] == "lw" || lineaMips[0] == "sw") {
        if (lineaMips.size() != 3 || registroMIPS.find(lineaMips[1]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere rt y offset(registro)" << endl;
            return;
        }
        rt = registroMIPS[lineaMips[1]];
        string offsetReg = lineaMips[2];
        int pos = offsetReg.find('(');
        if (pos == string::npos || offsetReg.back() != ')') {
            cout << "Error en PC 0x" << hex << pc << ": Formato invalido en '" << lineaMips[0] << "'" << endl;
            return;
        }
        int offset;
        try {
            if (offsetReg.find("0x") == 0) {
                offset = stoi(offsetReg.substr(0, pos), nullptr, 16);
            } else {
                offset = stoi(offsetReg.substr(0, pos));
            }
            cout << offset << endl;
            cout << offsetReg << endl;

        } catch (...) {
            cout << "Error en PC 0x" << hex << pc << ": Offset debe ser un numero valido" << endl;
            return;
        }
        string reg = offsetReg.substr(pos + 1, offsetReg.find(')') - pos - 1);
        if (registroMIPS.find(reg) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": Registro base invalido en '" << lineaMips[0] << "'" << endl;
            return;
        }
        if (lineaMips[1] == "$gp" && offset > 65536) {
            cout << "Error en PC 0x" << hex << pc << ": Offset fuera de rango (65536)" << endl;
        } else if (offset < -32768 || offset > 32767) {
            cout << "Error en PC 0x" << hex << pc << ": Offset fuera de rango (-32768 a 32767)" << endl;
            return;
        }
        rs = registroMIPS[reg];
        immediate = bitset<16>(offset).to_string();
    } else if (lineaMips[0] == "beq" || lineaMips[0] == "bne") {
        if (lineaMips.size() != 4 || registroMIPS.find(lineaMips[1]) == registroMIPS.end() ||
            registroMIPS.find(lineaMips[2]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere rs, rt y etiqueta" << endl;
            return;
        }
        rs = registroMIPS[lineaMips[1]];
        rt = registroMIPS[lineaMips[2]];
        if (etiquetas.find(lineaMips[3]) == etiquetas.end()) {
            cout << "Error en PC 0x" << hex << pc << ": Etiqueta '" << lineaMips[3] << "' no encontrada" << endl;
            return;
        }
        int offset = (etiquetas[lineaMips[3]] - (pc + 0x0004)) / 4;
        if (etiquetas[lineaMips[3]] == 0) {
            offset = 0;
        }
        cout << etiquetas[lineaMips[3]] << " " << hex <<pc << " " << pc + 0x0004 << endl;
        if (offset < -32768 || offset > 32767) {
            cout << "Error en PC 0x" << hex << pc << ": Offset de salto fuera de rango" << endl;
            return;
        }
        immediate = bitset<16>(offset & 0xFFFF).to_string();
        // cout << offset << endl;
        // cout << immediate << endl; // Doble complemento para direcciones
    } else if (lineaMips[0] == "lui") {
        if (lineaMips.size() != 3 || registroMIPS.find(lineaMips[1]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": 'lui' requiere rt y un inmediato" << endl;
            return;
        }
        rt = registroMIPS[lineaMips[1]];
        rs = "00000";
        int imm;
        try {
            if (lineaMips[2].find("0x") == 0) {
                imm = stoi(lineaMips[2], nullptr, 16);
            } else {
                imm = stoi(lineaMips[2]);
            }
        } catch (...) {
            cout << "Error en PC 0x" << hex << pc << ": Inmediato debe ser un numero valido" << endl;
            return;
        }
        if (imm < 0 || imm > 65535) {
            cout << "Error en PC 0x" << hex << pc << ": Inmediato fuera de rango (0 a 65535)" << endl;
            return;
        }
        immediate = bitset<16>(imm).to_string();
    } else {
        if (lineaMips.size() != 4 || registroMIPS.find(lineaMips[1]) == registroMIPS.end() ||
            registroMIPS.find(lineaMips[2]) == registroMIPS.end()) {
            cout << "Error en PC 0x" << hex << pc << ": '" << lineaMips[0] << "' requiere rt, rs y un inmediato" << endl;
            return;
        }
        rt = registroMIPS[lineaMips[1]];
        rs = registroMIPS[lineaMips[2]];
        int imm;
        try {
            if (lineaMips[3].find("0x") == 0) {
                imm = stoi(lineaMips[3], nullptr, 16);
            } else {
                imm = stoi(lineaMips[3]);
            }
        } catch (...) {
            cout << "Error en PC 0x" << hex << pc << ": Inmediato debe ser un numero valido" << endl;
            return;
        }
        if (imm < -32768 || imm > 32767) {
            cout << "Error en PC 0x" << hex << pc << ": Inmediato fuera de rango (-32768 a 32767)" << endl;
            return;
        }
        immediate = bitset<16>(imm).to_string();
    }

    string binario = opcode + rs + rt + immediate;
    cout << "0x" << hex << pc << ": " << binario << endl;
    comandoCompleto += binario;
}

/**
 * @brief Esta funcion traduce de MIPS a instrucciones tipo J
 * @param lineaMips
 */
void traducirTipoJ(vector<string> lineaMips, int& pc) {
    string opcode = instruccionesMips[lineaMips[0]][1];
    if (lineaMips.size() != 2 || etiquetas.find(lineaMips[1]) == etiquetas.end()) {
        cout << "Error en PC 0x" << hex << pc << ": " << lineaMips[0] << "' requiere una etiqueta valida" << endl;
        return;
    }
    int direccion = etiquetas[lineaMips[1]] >> 2;
    string direccionBinaria = bitset<26>(direccion).to_string();

    string binario = opcode + direccionBinaria;
    cout << "0x" << hex << pc << ": " << binario << endl;
    comandoCompleto += binario;
}