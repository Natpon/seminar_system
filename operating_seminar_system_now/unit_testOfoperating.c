#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "operating_seminar_system_now/operating_seminar_system.h"  // ใช้ prototype
void test_add_seminar() {
    // สร้างไฟล์ input จำลอง
    FILE *input_file = fopen("test_input.txt", "w");
    fprintf(input_file, "seminar1\nspeaker1\n2023-10-10\n10:00\n12:00\n1\n50\n");
    fclose(input_file);

    // redirect stdin ไปอ่านจากไฟล์ test_input.txt
    freopen("test_input.txt", "r", stdin);

    // เรียกฟังก์ชันจริงที่อยู่ใน operating_seminar_system.c
    add_seminar();