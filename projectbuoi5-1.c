#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h> // Them thu vien nay de su dung isdigit

#define MAX_TRIPS 50
#define MAX_TICKETS 100

#define STATUS_NORMAL   0
#define STATUS_LOCKED   1
#define STATUS_CANCELED 2

// ========================= STRUCTS =========================

// Thong tin dia diem
typedef struct {
    char name[50];    // ten dia diem 
    char address[100];  // dia chi cu the
} Location;

// Thong tin chuyen xe
typedef struct {
    char tripID[20];    // ma chuyen xe
    Location departure; // diem khoi hanh
    Location destination; // diem den
    char date[30];      // ngay gio (dd/mm/yy hh:mm)
    int totalSeats;    // tong so ghe
    int bookedSeats;   // so ghe da dat
} Trip;

// Thong tin hanh khach
typedef struct {
    char name[50];      // ten hanh khach
    char phone[15];     // so dien thoai
} Passenger;

// Thong tin ve xe
typedef struct {
    char tiketId[20];   // ma ve
    char tripId[20];    // ma chuyen xe
    Passenger passenger; // thong tin hanh khach
    int seatNumber;     // so ghe
    double price;       // gia ve
    int paymentStatus;  // 0: chua TT, 1: da TT
    int status;         // 0: bt, 1:khoa, 2:huy
    char date[30];      // ngay dat ve (dd/mm/yy hh:mm)
} Ticket;

// Thong tin chuyen xe cho bao cao
typedef struct {
    char tripID[20];
    int total_tickets;
    int paid_tickets;
    int cancelled_tickets;
    int valid_tickets;
    double revenue;
} TripReport;

// ========================= KHAI BAO HAM =========================

void printMenu();
int input_choice(int min, int max);
// Ham input da duoc sua de KHONG in thong bao loi khi de trong, chi nhan gia tri
void input_strict(char dest[], int max_len); 
void input_datetime(char dest[]);
bool validate_datetime(const char *dt);
void clear_input();
int check_ID(Trip trip_list[], int num_trip, char *id);

// Ham tien ich moi de nhap so nguyen duong
int input_positive_integer(const char *prompt); 

void addBusTrip(Trip trip_list[], int *num_trips);
void update_bus_trip(Trip trip_list[], int num_trips);
void book_ticket(Trip trip_list[], int num_trips, Ticket ticket_list[], int *num_tickets);
void check_ticket_status(Ticket ticket_list[], int num_tickets);
void display_trips_with_pagination(Trip trip_list[], int num_trips); 
void payment_ticket(Ticket ticket_list[], int num_tickets);
void manage_ticket_status(Ticket ticket_list[], int num_tickets, Trip trip_list[], int num_trips);

// Bao cao thong ke
void report_statistic(Ticket ticket_list[], int num_tickets, Trip trip_list[], int num_trips); 
int compare_datetimes(const char *dt1, const char *dt2);
bool is_ticket_in_range(const Ticket *ticket, const char *fromDate, const char *toDate);

// ========================= MAIN =========================

int main() {
    Trip trip_list[MAX_TRIPS];
    Ticket ticket_list[MAX_TICKETS];
    int num_trips = 0;
    int num_tickets = 0;

    // Khoi tao du lieu mau neu can de kiem tra:
    // Trip t1 = {"TX001", {"HCM", "TP HCM"}, {"HN", "Ha Noi"}, "31/12/25 20:00", 40, 5};
    // trip_list[num_trips++] = t1;
    // Trip t2 = {"TX002", {"DN", "Da Nang"}, {"CT", "Can Tho"}, "01/01/26 10:30", 30, 0};
    // trip_list[num_trips++] = t2;
    // Ticket tk1 = {"V001", "TX001", {"Nguyen Van A", "0901234567"}, 10, 500000.0, 1, STATUS_NORMAL, "20/11/25 15:00"};
    // ticket_list[num_tickets++] = tk1;
    // Ticket tk2 = {"V002", "TX001", {"Tran Van B", "0901234568"}, 20, 600000.0, 0, STATUS_NORMAL, "21/11/25 10:00"};
    // ticket_list[num_tickets++] = tk2;
    // Ticket tk3 = {"V003", "TX001", {"Pham Van C", "0901234569"}, 25, 500000.0, 1, STATUS_CANCELED, "22/11/25 09:00"};
    // ticket_list[num_tickets++] = tk3;
    // Ticket tk4 = {"V004", "TX002", {"Le Thi D", "0901234570"}, 5, 400000.0, 1, STATUS_NORMAL, "23/11/25 12:00"};
    // ticket_list[num_tickets++] = tk4;


    while (1) {
        printMenu();
        int choice = input_choice(1, 8);

        switch (choice) {
            case 1: addBusTrip(trip_list, &num_trips); break;
            case 2: update_bus_trip(trip_list, num_trips); break;
            case 3: book_ticket(trip_list, num_trips, ticket_list, &num_tickets); break;
            case 4: check_ticket_status(ticket_list, num_tickets); break;
            case 5: display_trips_with_pagination(trip_list, num_trips); break; 
            case 6: payment_ticket(ticket_list, num_tickets); break;
            case 7: manage_ticket_status(ticket_list, num_tickets, trip_list, num_trips); break;
            case 8: report_statistic(ticket_list, num_tickets, trip_list, num_trips); break; 
        }
    }

    return 0;
}

// ========================= UTILITIES =========================

void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Ham input_strict: khong cho phep de trong, in thong bao loi
void input_strict(char dest[], int max_len) {
    while (1) {
        if (fgets(dest, max_len, stdin) != NULL) {
            dest[strcspn(dest, "\n")] = 0;

            if (strlen(dest) == 0) {
                printf("[Loi] Khong duoc de trong. Nhap lai: ");
                continue;
            }
            return;
        }
    }
}

// Ham input: chi nhan gia tri, cho phep de trong de su dung trong report_statistic
void input(char dest[], int max_len) {
    if (fgets(dest, max_len, stdin) != NULL) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        // Truong hop doc loi
        clear_input();
        dest[0] = 0; // Gan chuoi rong
    }
}


// Ham moi de nhap so nguyen duong
int input_positive_integer(const char *prompt) {
    char buffer[20];
    while (1) {
        printf("%s", prompt);
        
        // Su dung input_strict vi cac truong hop nay luon can gia tri > 0
        input_strict(buffer, 20);


        bool is_digit = true;
        for (int i = 0; buffer[i]; i++) {
            // Chi chap nhan chu so. Ký tu '.' se bi tu choi
            if (!isdigit((unsigned char)buffer[i])) { 
                is_digit = false;
                break;
            }
        }

        if (!is_digit) {
            printf("[Loi] Chi duoc nhap so nguyen duong!\n"); 
            continue;
        }

        int num = atoi(buffer);
        if (num <= 0) {
            printf("[Loi] So phai > 0!\n");
            continue;
        }
        return num;
    }
}


bool validate_datetime(const char *dt) {
    int d, m, y, hh, mm;
    if (sscanf(dt, "%2d/%2d/%2d %2d:%2d", &d, &m, &y, &hh, &mm) != 5)
        return false;
    if (d < 1 || d > 31) return false;
    if (m < 1 || m > 12) return false;
    if (y < 0 || y > 99) return false;
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

void input_datetime(char dest[]) {
    while (1) {
        input_strict(dest, 30);
        if (!validate_datetime(dest)) {
            printf("[Loi] Dinh dang dung: dd/mm/yy hh:mm. Vi du: 31/12/24 15:30\nNhap lai: ");
        } else return;
    }
}

int input_choice(int min, int max) {
    char buffer[10];
    while (1) {
        printf("Moi ban nhap lua chon (%d-%d): ", min, max);
        input_strict(buffer, 10);

        bool is_digit = true;
        for (int i = 0; buffer[i]; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                is_digit = false;
                break;
            }
        }

        if (!is_digit) {
            printf("[Loi] Chi duoc nhap so!\n");
            continue;
        }

        int num = atoi(buffer);
        if (num < min || num > max) {
            printf("[Loi] Lua chon phai trong khoang %d-%d!\n", min, max);
            continue;
        }

        return num;
    }
}

void printMenu() {
    printf("\n+------------------------------------------------------------+\n");
    printf("|           MENU CHUC NANG                                   |\n");
    printf("+------------------------------------------------------------+\n");
    printf("| 1. Them chuyen xe moi                                      |\n");
    printf("| 2. Cap nhat thong tin chuyen xe                            |\n");
    printf("| 3. Dat ve                                                  |\n");
    printf("| 4. Kiem tra tinh trang ve                                  |\n");
    printf("| 5. Liet ke chuyen xe theo trang                            |\n");
    printf("| 6. Thanh toan ve                                           |\n");
    printf("| 7. Quan ly trang thai ve (Khoa/Huy)                        |\n");
    printf("| 8. Bao cao thong ke                                        |\n");
    printf("+------------------------------------------------------------+\n");
}

int check_ID(Trip trip_list[], int num_trip, char *id){
    for (int i = 0; i < num_trip; i++){
        if (strcmp(trip_list[i].tripID, id) == 0)
            return i;
    }
    return -1;
}

// ========================= UTILITIES - DATETIME =========================

// So sanh hai chuoi ngay gio (dd/mm/yy hh:mm)
// Tra ve < 0 neu dt1 < dt2, = 0 neu dt1 = dt2, > 0 neu dt1 > dt2
int compare_datetimes(const char *dt1, const char *dt2) {
    int d1, m1, y1, hh1, mm1;
    int d2, m2, y2, hh2, mm2;

    if (sscanf(dt1, "%2d/%2d/%2d %2d:%2d", &d1, &m1, &y1, &hh1, &mm1) != 5) return 0;
    if (sscanf(dt2, "%2d/%2d/%2d %2d:%2d", &d2, &m2, &y2, &hh2, &mm2) != 5) return 0;

    // So sanh theo thu tu: Nam (y), Thang (m), Ngay (d), Gio (hh), Phut (mm)
    // Chuyen nam 2 chu so sang 4 chu so
    int full_y1 = y1 + 2000;
    int full_y2 = y2 + 2000;

    if (full_y1 != full_y2) return full_y1 - full_y2;
    if (m1 != m2) return m1 - m2;
    if (d1 != d2) return d1 - d2;
    if (hh1 != hh2) return hh1 - hh2;
    return mm1 - mm2;
}

// Kiem tra ve co nam trong khoang thoi gian [fromDate, toDate] hay khong
bool is_ticket_in_range(const Ticket *ticket, const char *fromDate, const char *toDate) {
    if (!validate_datetime(ticket->date)) return false; // Ve khong co ngay dat hop le

    // fromDate luon co gia tri hop le
    bool after_from = (compare_datetimes(ticket->date, fromDate) >= 0);
    
    bool before_to = true;
    if (strlen(toDate) > 0) {
        // toDate co the co gia tri nhung khong hop le (da duoc canh bao truoc do)
        if (!validate_datetime(toDate)) return false; 
        // ticket.date <= toDate
        before_to = (compare_datetimes(ticket->date, toDate) <= 0);
    }

    return after_from && before_to;
}

// ========================= THEM CHUYEN XE =========================

void addBusTrip(Trip trip_list[], int *num_trips) {
    if (*num_trips >= MAX_TRIPS) {
        printf("[Loi] Danh sach da day!\n");
        return;
    }

    Trip new_trip;
    char temp_id[20];

    printf("\n--- THEM CHUYEN XE MOI ---\n");

    while (1) {
        printf("Nhap ma chuyen xe: ");
        input_strict(temp_id, 20);
        if (check_ID(trip_list, *num_trips, temp_id) != -1)
            printf("[Loi] Ma chuyen xe da ton tai!\n");
        else break;
    }

    strcpy(new_trip.tripID, temp_id);

    printf("Nhap ten diem khoi hanh: ");
    input_strict(new_trip.departure.name, 50);

    printf("Nhap dia chi khoi hanh: ");
    input_strict(new_trip.departure.address, 100);

    printf("Nhap ten diem den: ");
    input_strict(new_trip.destination.name, 50);

    printf("Nhap dia chi diem den: ");
    input_strict(new_trip.destination.address, 100);

    printf("Nhap ngay gio chuyen xe (dd/mm/yy hh:mm): ");
    input_datetime(new_trip.date);

    // Su dung ham moi de chi nhan so nguyen duong
    new_trip.totalSeats = input_positive_integer("Nhap tong so ghe: ");

    new_trip.bookedSeats = 0;
    trip_list[*num_trips] = new_trip;
    (*num_trips)++;

    printf("[Thanh cong] Da them chuyen xe!\n");
}

// ========================= CAP NHAT CHUYEN XE =========================

void update_bus_trip(Trip trip_list[], int num_trips) {
    if (num_trips == 0) {
        printf("[Loi] Khong co chuyen xe nao!\n");
        return;
    }

    char id[20];
    printf("\n--- CAP NHAT CHUYEN XE ---\n");
    printf("Nhap ma chuyen xe: ");
    input_strict(id, 20);

    int index = check_ID(trip_list, num_trips, id);
    if (index == -1) {
        printf("[Loi] Khong tim thay chuyen xe!\n");
        return;
    }

    Trip *trip = &trip_list[index];

    printf("Nhap ten diem khoi hanh moi: ");
    input_strict(trip->departure.name, 50);

    printf("Nhap dia chi khoi hanh moi: ");
    input_strict(trip->departure.address, 100);

    printf("Nhap ten diem den moi: ");
    input_strict(trip->destination.name, 50);

    printf("Nhap dia chi diem den moi: ");
    input_strict(trip->destination.address, 100);

    printf("Nhap ngay gio chuyen xe moi (dd/mm/yy hh:mm): ");
    input_datetime(trip->date);

    // Su dung ham moi de chi nhan so nguyen duong
    while (1) {
        int seat = input_positive_integer("Nhap tong so ghe moi: ");
        
        if (seat < trip->bookedSeats) {
            printf("[Loi] So ghe moi khong duoc nho hon so ghe da dat (%d)!\n", trip->bookedSeats);
        } else {
            trip->totalSeats = seat;
            break;
        }
    }

    printf("[Thanh cong] Cap nhat thanh cong!\n");
}

// ========================= DAT VE =========================

void book_ticket(Trip trip_list[], int num_trips, Ticket ticket_list[], int *num_tickets) {
    if (num_trips == 0) {
        printf("[Loi] Khong co chuyen xe!\n");
        return;
    }
    if (*num_tickets >= MAX_TICKETS) {
        printf("[Loi] Het cho nhan ve!\n");
        return;
    }

    char trip_id[20];
    printf("\n--- DAT VE ---\n");
    printf("Nhap ma chuyen xe: ");
    input_strict(trip_id, 20);

    int idx = check_ID(trip_list, num_trips, trip_id);
    if (idx == -1) {
        printf("[Loi] Khong tim thay chuyen xe!\n");
        return;
    }

    Trip *trip = &trip_list[idx];

    if (trip->bookedSeats >= trip->totalSeats) {
        printf("[Loi] Chuyen xe da het ghe!\n");
        return;
    }

    Ticket new_ticket;
    strcpy(new_ticket.tripId, trip_id);

    printf("Nhap ten hanh khach: ");
    input_strict(new_ticket.passenger.name, 50);

    printf("Nhap so dien thoai: ");
    input_strict(new_ticket.passenger.phone, 15);

    while (1) {
        int seat = input_positive_integer("Nhap so ghe: ");
        
        if (seat > trip->totalSeats) {
            printf("[Loi] So ghe phai tu 1-%d!\n", trip->totalSeats);
            continue;
        }

        bool taken = false;
        for (int i = 0; i < *num_tickets; i++) {
            if (strcmp(ticket_list[i].tripId, trip_id) == 0 &&
                ticket_list[i].seatNumber == seat &&
                ticket_list[i].status == STATUS_NORMAL) {
                taken = true;
                break;
            }
        }

        if (taken) printf("[Loi] Ghe da duoc dat!\n");
        else {
            new_ticket.seatNumber = seat;
            break;
        }
    }

    while (1) {
        char buffer[20];
        printf("Nhap gia ve: ");
        input_strict(buffer, 20);

        double price = atof(buffer);
        // Them kiem tra hop le cho gia ve (duong)
        if (price <= 0) printf("[Loi] Gia ve phai > 0!\n");
        else {
            new_ticket.price = price;
            break;
        }
    }

    printf("Nhap ngay dat ve (dd/mm/yy hh:mm): ");
    input_datetime(new_ticket.date);

    sprintf(new_ticket.tiketId, "V%03d", *num_tickets + 1);
    new_ticket.paymentStatus = 0;
    new_ticket.status = STATUS_NORMAL;

    ticket_list[*num_tickets] = new_ticket;
    (*num_tickets)++;
    trip->bookedSeats++;

    printf("\n[Thanh cong] Dat ve thanh cong!\n");
    printf("=== THONG TIN VE ===\n");
    printf("Ma ve: %s\n", new_ticket.tiketId);
    printf("Chuyen xe: %s\n", new_ticket.tripId);
    printf("Hanh khach: %s\n", new_ticket.passenger.name);
    printf("SDT: %s\n", new_ticket.passenger.phone);
    printf("So ghe: %d\n", new_ticket.seatNumber);
    printf("Gia ve: %.2lf\n", new_ticket.price);
    printf("Ngay dat: %s\n", new_ticket.date);
    printf("====================\n");
}

// ========================= KIEM TRA VE =========================

void check_ticket_status(Ticket ticket_list[], int num_tickets) {
    if (num_tickets == 0) {
        printf("[Thong bao] Chua co ve nao!\n");
        return;
    }

    char id[20];
    printf("\n--- KIEM TRA VE ---\n");
    printf("Nhap ma ve: ");
    input_strict(id, 20);

    for (int i = 0; i < num_tickets; i++) {
        if (strcmp(ticket_list[i].tiketId, id) == 0) {
            Ticket *tk = &ticket_list[i];

            printf("\nMa ve: %s\n", tk->tiketId);
            printf("Hanh khach: %s\n", tk->passenger.name);
            printf("SDT: %s\n", tk->passenger.phone);
            printf("Chuyen xe: %s\n", tk->tripId);
            printf("So ghe: %d\n", tk->seatNumber);
            printf("Gia: %.2lf\n", tk->price);
            printf("Thanh toan: %s\n", tk->paymentStatus ? "Da thanh toan" : "Chua thanh toan");

            printf("Trang thai: ");
            if (tk->status == STATUS_NORMAL) printf("Binh thuong\n");
            else if (tk->status == STATUS_LOCKED) printf("Da khoa\n");
            else printf("Da huy\n");

            printf("Ngay dat: %s\n", tk->date);
            return;
        }
    }

    printf("[Loi] Khong tim thay ve!\n");
}

// ========================= HIEN THI THEO TRANG (CASE 5) =========================

void display_trips_with_pagination(Trip trip_list[], int num_trips) {
    if (num_trips == 0) {
        printf("[Thong bao] Khong co chuyen xe de hien thi\n");
        return;
    }

    int pageSize = 10; // Gia tri mac dinh
    int currentPage = 1;
    char command[10];

    printf("\n--- LIET KE CHUYEN XE ---\n");
    
    // Yeu cau nguoi dung nhap pageSize (cho phep Enter de dung mac dinh 10)
    printf("Nhap so luong chuyen xe tren moi trang (Mac dinh: 10, Nhap so > 0 hoac Enter de dung mac dinh): ");
    char pageSizeBuffer[10];
    input(pageSizeBuffer, 10); // Su dung input de cho phep nhap Enter

    if (strlen(pageSizeBuffer) > 0) {
        bool is_digit = true;
        for (int i = 0; pageSizeBuffer[i]; i++) {
            if (!isdigit((unsigned char)pageSizeBuffer[i])) { 
                is_digit = false;
                break;
            }
        }
        if (is_digit) {
            int input_pageSize = atoi(pageSizeBuffer);
            if (input_pageSize > 0) {
                pageSize = input_pageSize;
            } else {
                printf("[Canh bao] So trang khong hop le hoac <= 0, su dung mac dinh: 10.\n");
            }
        } else {
             printf("[Canh bao] Nhap lieu khong phai so, su dung mac dinh: 10.\n");
        }
    }


    int totalPages = (num_trips + pageSize - 1) / pageSize;

    while (1) {
        if (totalPages == 0) { 
            printf("[Thong bao] Khong co chuyen xe de hien thi\n");
            break;
        }

        if (currentPage < 1) currentPage = 1;
        if (currentPage > totalPages) currentPage = totalPages;

        int start = (currentPage - 1) * pageSize;
        int end = start + pageSize;
        if (end > num_trips) end = num_trips;

        printf("\n+-----------------------------------------------------------------------------------------------------------------+\n");
        printf("|           DANH SACH CHUYEN XE - Trang %d/%d (Hien thi %d/%d chuyen xe)                               |\n", currentPage, totalPages, (end - start), num_trips);
        printf("+----------+--------------------------+--------------------------+---------------------+-----------------+\n");
        printf("| %-8s | %-24s | %-24s | %-19s | %-15s |\n",
                 "tripId", "Noi khoi hanh", "Noi den", "Ngay gio", "Booked/Total");
        printf("+----------+--------------------------+--------------------------+---------------------+-----------------+\n");

        for (int i = start; i < end; i++) {
            printf("| %-8s | %-24s | %-24s | %-19s | %-7d/%-7d |\n",
                      trip_list[i].tripID,
                      trip_list[i].departure.name,
                      trip_list[i].destination.name,
                      trip_list[i].date,
                      trip_list[i].bookedSeats,
                      trip_list[i].totalSeats
            );
        }
        printf("+----------+--------------------------+--------------------------+---------------------+-----------------+\n");

        // Huong dan dieu hung
        printf("\nDieu huong: 'n' (Next), 'p' (Previous), so trang (vd: 3), '0' (Thoat): ");
        input(command, 10);

        if (strcmp(command, "0") == 0) {
            printf("Da thoat chuc nang liet ke chuyen xe.\n");
            break;
        } else if (strcmp(command, "n") == 0 || strcmp(command, "N") == 0) {
            if (currentPage < totalPages) {
                currentPage++;
            } else {
                printf("[Thong bao] Day la trang cuoi cung!\n");
            }
        } else if (strcmp(command, "p") == 0 || strcmp(command, "P") == 0) {
            if (currentPage > 1) {
                currentPage--;
            } else {
                printf("[Thong bao] Day la trang dau tien!\n");
            }
        } else if (strlen(command) > 0) {
            // Nhap so trang
            bool is_valid_page_number = true;
            for (int i = 0; command[i]; i++) {
                if (!isdigit((unsigned char)command[i])) {
                    is_valid_page_number = false;
                    break;
                }
            }

            if (is_valid_page_number) {
                 int newPage = atoi(command);
                 if (newPage > 0 && newPage <= totalPages) {
                     currentPage = newPage;
                 } else {
                     printf("[Loi] So trang khong hop le (phai tu 1 den %d).\n", totalPages);
                 }
            } else {
                printf("[Loi] Lenh dieu huong khong hop le. Hay dung 'n', 'p', so trang, hoac '0'.\n");
            }
        }
    }
}

// ========================= THANH TOAN VE =========================

void payment_ticket(Ticket ticket_list[], int num_tickets) {
    if (num_tickets == 0) {
        printf("[Thong bao] Chua co ve nao!\n");
        return;
    }

    char id[20];
    printf("\n--- THANH TOAN VE ---\n");
    printf("Nhap ma ve: ");
    input_strict(id, 20);

    for (int i = 0; i < num_tickets; i++) {
        if (strcmp(ticket_list[i].tiketId, id) == 0) {
            Ticket *tk = &ticket_list[i];

            if (tk->status != STATUS_NORMAL) {
                printf("[Loi] Ve khong hop le de thanh toan!\n");
                return;
            }

            if (tk->paymentStatus) {
                printf("[Thong bao] Ve da duoc thanh toan!\n");
            } else {
                tk->paymentStatus = 1;
                printf("[Thanh cong] Da thanh toan ve %s\n", tk->tiketId);
            }
            return;
        }
    }

    printf("[Loi] Khong tim thay ve!\n");
}

// ========================= QUAN LY TRANG THAI VE =========================

void manage_ticket_status(Ticket ticket_list[], int num_tickets, Trip trip_list[], int num_trips) {
    if (num_tickets == 0) {
        printf("[Thong bao] Chua co ve nao!\n");
        return;
    }

    char id[20];
    printf("\n--- QUAN LY TRANG THAI VE ---\n");
    printf("Nhap ma ve: ");
    input_strict(id, 20);

    for (int i = 0; i < num_tickets; i++) {
        if (strcmp(ticket_list[i].tiketId, id) == 0) {
            Ticket *tk = &ticket_list[i];

            printf("Chon trang thai moi (0: Binh thuong, 1: Khoa, 2: Huy): ");
            int newStatus = input_choice(0, 2);

            // Logic cap nhat so ghe da dat
            if (tk->status == STATUS_NORMAL && newStatus == STATUS_CANCELED) {
                // Tru so ghe da dat khi chuyen tu Binh thuong -> Huy
                int trip_idx = check_ID(trip_list, num_trips, tk->tripId);
                if (trip_idx != -1) trip_list[trip_idx].bookedSeats--;
            } else if (tk->status == STATUS_CANCELED && newStatus == STATUS_NORMAL) {
                // Cong so ghe da dat khi chuyen tu Huy -> Binh thuong
                int trip_idx = check_ID(trip_list, num_trips, tk->tripId);
                if (trip_idx != -1) {
                    if (trip_list[trip_idx].bookedSeats < trip_list[trip_idx].totalSeats) {
                         trip_list[trip_idx].bookedSeats++;
                    } else {
                        printf("[Loi] Khong the phuc hoi: Chuyen xe da day!\n");
                        // Giu nguyen trang thai de tranh loi logic
                        return;
                    }
                }
            }


            tk->status = newStatus;
            printf("[Thanh cong] Cap nhat trang thai ve %s thanh cong!\n", tk->tiketId);
            return;
        }
    }

    printf("[Loi] Khong tim thay ve!\n");
}

// ========================= BAO CAO THONG KE (CASE 8 ÐÃ S?A Ð?I) =========================

void report_statistic(Ticket ticket_list[], int num_tickets, Trip trip_list[], int num_trips) {
    printf("\n--- BAO CAO THONG KE ---\n");

    if (num_tickets == 0) {
        printf("[Thong bao] Khong co du lieu de bao cao\n");
        return;
    }

    char fromDate[30] = "";
    char toDate[30] = "";
    char type_buffer[10];
    int type;

    // 1. Nhap ngay bat dau (BAT BUOC)
    while (1) {
        printf("Nhap ngay bat dau (dd/mm/yy hh:mm): ");
        input_strict(fromDate, 30); // Bat buoc nhap
        if (validate_datetime(fromDate)) {
            break;
        } else {
            printf("[Loi] Dinh dang ngay bat dau khong hop le. Hay nhap lai (dd/mm/yy hh:mm).\n");
        }
    }

    // 2. Nhap ngay ket thuc (KHONG BAT BUOC)
    printf("Nhap ngay ket thuc (dd/mm/yy hh:mm, Enter de khong gioi han): ");
    char toDateBuffer[30];
    input(toDateBuffer, 30);
    strcpy(toDate, ""); // Dat lai toDate la rong
    
    if (strlen(toDateBuffer) > 0) {
        if (!validate_datetime(toDateBuffer)) {
            printf("[Canh bao] Dinh dang ngay ket thuc khong hop le. Khong gioi han thoi gian ket thuc.\n");
        } else {
            strcpy(toDate, toDateBuffer);
            // Kiem tra fromDate <= toDate
            if (compare_datetimes(fromDate, toDate) > 0) {
                printf("[Loi] Ngay bat dau (%s) phai nho hon hoac bang ngay ket thuc (%s). Khong the tao bao cao.\n", fromDate, toDate);
                strcpy(toDate, ""); // Xoa toDate de khong co bao cao hop le
                return;
            }
        }
    }
    // Neu de trong thi toDate van la "" (khong gioi han)

    printf("Chon kieu bao cao (1: Tong doanh thu, 2: Theo chuyen xe, 3: Theo thoi gian): ");
    input_strict(type_buffer, 10);
    type = atoi(type_buffer);

    if (type < 1 || type > 3) {
        printf("[Loi] Loai bao cao khong hop le. Thoat bao cao.\n");
        return;
    }

    // Bien toan cuc cho bao cao theo pham vi
    double total_revenue = 0.0;
    int total_paid_tickets = 0;
    int total_tickets_in_range = 0;
    int total_cancelled_tickets = 0;
    int total_valid_tickets = 0;

    // Du lieu cho bao cao theo chuyen xe (Case 2)
    TripReport trip_reports[MAX_TRIPS] = {0};
    int num_trip_reports = 0;

    // Xay dung thong ke
    for (int i = 0; i < num_tickets; i++) {
        Ticket *tk = &ticket_list[i];

        // Chi loc ve co ngay dat hop le va trong pham vi [fromDate, toDate]
        if (!is_ticket_in_range(tk, fromDate, toDate)) continue;

        // Thong ke chung
        total_tickets_in_range++;
        if (tk->paymentStatus == 1) {
            total_paid_tickets++;
            total_revenue += tk->price;
        }

        if (tk->status == STATUS_CANCELED) {
            total_cancelled_tickets++;
        } else if (tk->status == STATUS_NORMAL) {
            total_valid_tickets++;
        }

        // Thong ke theo chuyen xe (Case 2)
        if (type == 2) {
            int found_idx = -1;
            for (int j = 0; j < num_trip_reports; j++) {
                if (strcmp(trip_reports[j].tripID, tk->tripId) == 0) {
                    found_idx = j;
                    break;
                }
            }

            if (found_idx == -1) {
                if (num_trip_reports < MAX_TRIPS) {
                    strcpy(trip_reports[num_trip_reports].tripID, tk->tripId);
                    found_idx = num_trip_reports;
                    num_trip_reports++;
                } else {
                    continue; // Bo qua neu mang bao cao da day
                }
            }

            // Cap nhat thong tin cho chuyen xe
            trip_reports[found_idx].total_tickets++;
            if (tk->paymentStatus == 1) {
                trip_reports[found_idx].paid_tickets++;
                trip_reports[found_idx].revenue += tk->price;
            }
            if (tk->status == STATUS_CANCELED) {
                trip_reports[found_idx].cancelled_tickets++;
            } else if (tk->status == STATUS_NORMAL) {
                trip_reports[found_idx].valid_tickets++;
            }
        }
    }
    
    // Xuat bao cao
    printf("\n");
    if (total_tickets_in_range == 0) {
          printf("[Thong bao] Khong co ve nao trong khoang thoi gian da chon.\n");
          return;
    }

    char range_info[100];
    if (strlen(toDate) > 0)
        sprintf(range_info, "tu %s den %s", fromDate, toDate);
    else
        sprintf(range_info, "tu %s den hien tai (khong gioi han ket thuc)", fromDate);


    switch (type) {
        case 1: // Tong doanh thu
            printf("--- BAO CAO TONG DOANH THU (%s) ---\n", range_info);
            printf("Tong doanh thu: %.2lf VNÐ\n", total_revenue);
            printf("Tong ve da thanh toan: %d\n", total_paid_tickets);
            printf("Tong ve trong pham vi loc: %d\n", total_tickets_in_range);
            break;

        case 2: // Thong ke theo chuyen xe
            printf("--- BAO CAO THEO CHUYEN XE (%s) ---\n", range_info);
            printf("+---------------------------------------------------------------------------------------------------+\n");
            printf("| %-8s | %-10s | %-15s | %-8s | %-13s | %-15s |\n",
                      "tripId", "Tong ve", "Da thanh toan", "Huy", "Con hieu luc", "Doanh thu");
            printf("+---------------------------------------------------------------------------------------------------+\n");

            for (int i = 0; i < num_trip_reports; i++) {
                printf("| %-8s | %-10d | %-15d | %-8d | %-13d | %-15.2lf |\n",
                          trip_reports[i].tripID,
                          trip_reports[i].total_tickets,
                          trip_reports[i].paid_tickets,
                          trip_reports[i].cancelled_tickets,
                          trip_reports[i].valid_tickets,
                          trip_reports[i].revenue);
            }
            printf("+---------------------------------------------------------------------------------------------------+\n");
            break;

        case 3: // Thong ke theo khoang thoi gian
            printf("--- BAO CAO THEO KHOANG THOI GIAN (%s) ---\n", range_info);
            printf("Tong ve trong khoang thoi gian: %d\n", total_tickets_in_range);
            printf("Tong doanh thu: %.2lf VNÐ\n", total_revenue);
            printf("So ve da thanh toan: %d\n", total_paid_tickets);
            printf("So ve huy: %d\n", total_cancelled_tickets);
            printf("So ve con hieu luc (Binh thuong, chua huy/khoa): %d\n", total_valid_tickets);
            break;
    }
}
