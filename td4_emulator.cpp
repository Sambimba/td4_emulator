#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int instruction_pointer = 0;
int overflow_flag = 0;
int reg_a = 0;
int reg_b = 0;
int output_port = 0;

// перевод двоичной строки в десятичное
int binary_to_decimal(const string& bin) {
    int decimal = 0;
    int power = 0;
    for (int i = bin.length() - 1; i >= 0; --i) {
        if (bin[i] == '1') {
            decimal += (1 << power);
        }
        power++;
    }
    return decimal;
}

// перевод числа в двоичную строку
string to_binary(int value, int bits) {
    string binary = "";
    for (int i = bits - 1; i >= 0; --i) {
        binary += ((value >> i) & 1) ? '1' : '0';
    }
    return binary;
}

void print_state(const string& cmd) {
    cout << "A: " << to_binary(reg_a, 4)
         << "\tB: " << to_binary(reg_b, 4)
         << "\tc: " << overflow_flag
         << "\tPC: " << to_binary(instruction_pointer, 4)
         << "\tOUT: " << to_binary(output_port, 4)
         << "\tCMD: " << cmd << endl;
}

int add_with_overflow(int reg, int imm) {
    int result = reg + imm;
    if (result > 15) {
        overflow_flag = 1;
        return result - 16;
    } else {
        overflow_flag = 0;
        return result;
    }
}

void execute_command(const vector<string>& program) {
    if (instruction_pointer >= program.size()) {
        cout << "Программа завершена.\n";
        exit(0);
    }

    string instruction = program[instruction_pointer];
    if (instruction.length() < 8) {
        cout << "Неверная длина команды: " << instruction << endl;
        instruction_pointer++;
        return;
    }

    string opcode = instruction.substr(0, 4);
    string imm_str = instruction.substr(4, 4);
    int imm = binary_to_decimal(imm_str);

    if (opcode == "0000") {
        string cmd = "ADD A," + imm_str;
        print_state(cmd);
        reg_a = add_with_overflow(reg_a, imm);
        instruction_pointer++;
    }
    else if (opcode == "0101") {
        string cmd = "ADD B," + imm_str;
        print_state(cmd);
        reg_b = add_with_overflow(reg_b, imm);
        instruction_pointer++;
    }
    else if (opcode == "0011") {
        string cmd = "MOV A," + imm_str;
        print_state(cmd);
        reg_a = imm;
        overflow_flag = 0;
        instruction_pointer++;
    }
    else if (opcode == "0111") {
        string cmd = "MOV B," + imm_str;
        print_state(cmd);
        reg_b = imm;
        overflow_flag = 0;
        instruction_pointer++;
    }
    else if (opcode == "0001") {
        print_state("MOV A,B");
        reg_a = reg_b;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "0100") {
        print_state("MOV B,A");
        reg_b = reg_a;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "0010") {
        print_state("IN A");
        cout << "Введите значение для A: ";
        cin >> reg_a;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "0110") {
        print_state("IN B");
        cout << "Введите значение для B: ";
        cin >> reg_b;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "1001") {
        print_state("OUT B");
        output_port = reg_b;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "1011") {
        string cmd = "OUT " + imm_str;
        print_state(cmd);
        output_port = imm;
        instruction_pointer++;
        overflow_flag = 0;
    }
    else if (opcode == "1111") {
        string cmd = "JMP " + imm_str;
        print_state(cmd);
        instruction_pointer = imm;
        overflow_flag = 0;
    }
    else if (opcode == "1110") {
        string cmd = "JNC " + imm_str;
        print_state(cmd);
        if (overflow_flag == 0) {
            instruction_pointer = imm;
        } else {
            instruction_pointer++;
            overflow_flag = 0;
        }
    }
    else {
        print_state("NOP");
        instruction_pointer++;
        overflow_flag = 0;
    }
}

void run_auto_mode(const vector<string>& program, int freq) {
    while (true) {
        execute_command(program);
        usleep(1000000 / freq);
    }
}

void run_manual_mode(const vector<string>& program) {
    char input;
    while (true) {
        cin.get(input);
        if (input == 'p') {
            cout << "Пауза. Нажмите Enter для продолжения.\n";
            cin.get();
            continue;
        }
        execute_command(program);
    }
}

int main() {
    cout << "Введите количество команд: ";
    int n;
    cin >> n;

    vector<string> program(n);
    for (int i = 0; i < n; ++i) {
        cout << "Команда " << i << " (8 бит): ";
        cin >> program[i];
    }

    int mode;
    cout << "Выберите режим:\n1 - ручной\n2 - автоматический\n";
    cin >> mode;

    if (mode == 1) {
        run_manual_mode(program);
    } else if (mode == 2) {
        int freq;
        cout << "Частота (1 или 10): ";
        cin >> freq;
        run_auto_mode(program, freq * 1000000);
    }

    return 0;
}
