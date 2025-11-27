#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_TRIPS 50
#define MAX_TICKETS 100

#define STATUS_NORMAL   0
#define STATUS_LOCKED   1
#define STATUS_CANCELED 2

// ========================= STRUCTS =========================

// Thong tin dia diem
typedef struct {
    char name[50];        // ten dia diem 
    char address[100];    // dia chi cu the
} Location;

// Thong tin chuyen xe
typedef struct {
    char tripID[20];      // ma chuyen xe
    Location departure;   // diem khoi hanh
    Location destination; // diem den
    char date[30];        // ngay gio (dd/mm/yy hh:mm)
    int totalSeats;       // tong so ghe
    int bookedSeats;      // so ghe da dat
} Trip;

// Thong tin hanh khach
typedef struct {
    char name[50];        // ten hanh khach
    char phone[15];       // so dien thoai
} Passenger;

// Thong tin ve xe
typedef struct {
    char tiketId[20];     // ma ve
    char tripId[20];      // ma chuyen xe
    Passenger passenger;  // thong tin hanh khach
    int seatNumber;       // so ghe
    double price;         // gia ve
    int paymentStatus;    // 0: chua TT, 1: da TT
    int status;           // 0: bt, 1:khoa, 2:huy
    char date[30];        // ngay dat ve (dd/mm/yy hh:mm)
} Ticket;

// ========================= KHAI BÁO HÀM =========================

void printMenu();
int input_choice(int min, int max);
void input(char dest[], int max_len);
void input_datetime(char dest[]);
bool validate_datetime(const char *dt);
void clear_input();
int check_ID(Trip trip_list[], int num_trip, char *id);

void addBusTrip(Trip trip_list[], int *num_trips);
void update_bus_trip(Trip trip_list[], int num_trips);
void book_ticket(Trip trip_list[], int num_trips, Ticket ticket_list[], int *num_tickets);
void check_ticket_status(Ticket ticket_list[], int num_tickets);
void display_trips_with_pagination(Trip trip_list[], int num_trips);
void payment_ticket(Ticket ticket_list[], int num_tickets);
void manage_ticket_status(Ticket ticket_list[], int num_tickets, Trip trip_list[], int num_trips);

// ========================= MAIN =========================

int main() {
    Trip trip_list[MAX_TRIPS];
    Ticket ticket_list[MAX_TICKETS];
    int num_trips = 0;
    int num_tickets = 0;

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
            case 8: printf("Chuc nang dang phat trien!\n"); break;
        }
    }

    return 0;
}

// ========================= UTILITIES =========================

void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void input(char dest[], int max_len) {
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
        input(dest, 30);
        if (!validate_datetime(dest)) {
            printf("[Loi] Dinh dang dung: dd/mm/yy hh:mm. Vi du: 31/12/24 15:30\nNhap lai: ");
        } else return;
    }
}

int input_choice(int min, int max) {
    char buffer[10];
    while (1) {
        printf("Moi ban nhap lua chon (%d-%d): ", min, max);
        input(buffer, 10);

        bool is_digit = true;
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
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
    printf("|                      MENU CHUC NANG                        |\n");
    printf("+------------------------------------------------------------+\n");
    printf("| 1. Them chuyen xe moi                                      |\n");
    printf("| 2. Cap nhat thong tin chuyen xe                            |\n");
    printf("| 3. Dat ve                                                  |\n");
    printf("| 4. Kiem tra tinh trang ve                                  |\n");
    printf("| 5. Liet ke chuyen xe theo trang                            |\n");
    printf("| 6. Thanh toan ve                                           |\n");
    printf("| 7. Quan ly trang thai ve (Khoa/Huy)                        |\n");
    printf("| 8. Bao cao thong ke (chua hoan thien)                      |\n");
    printf("+------------------------------------------------------------+\n");
}

int check_ID(Trip trip_list[], int num_trip, char *id){
    for (int i = 0; i < num_trip; i++){
        if (strcmp(trip_list[i].tripID, id) == 0)
            return i;
    }
    return -1;
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
        input(temp_id, 20);
        if (check_ID(trip_list, *num_trips, temp_id) != -1)
            printf("[Loi] Ma chuyen xe da ton tai!\n");
        else break;
    }

    strcpy(new_trip.tripID, temp_id);

    printf("Nhap ten diem khoi hanh: ");
    input(new_trip.departure.name, 50);

    printf("Nhap dia chi khoi hanh: ");
    input(new_trip.departure.address, 100);

    printf("Nhap ten diem den: ");
    input(new_trip.destination.name, 50);

    printf("Nhap dia chi diem den: ");
    input(new_trip.destination.address, 100);

    printf("Nhap ngay gio chuyen xe (dd/mm/yy hh:mm): ");
    input_datetime(new_trip.date);

    while (1) {
        char buffer[20];
        printf("Nhap tong so ghe: ");
        input(buffer, 20);

        int seat = atoi(buffer);
        if (seat <= 0) printf("[Loi] So ghe phai > 0!\n");
        else {
            new_trip.totalSeats = seat;
            break;
        }
    }

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
    input(id, 20);

    int index = check_ID(trip_list, num_trips, id);
    if (index == -1) {
        printf("[Loi] Khong tim thay chuyen xe!\n");
        return;
    }

    Trip *trip = &trip_list[index];

    printf("Nhap ten diem khoi hanh moi: ");
    input(trip->departure.name, 50);

    printf("Nhap dia chi khoi hanh moi: ");
    input(trip->departure.address, 100);

    printf("Nhap ten diem den moi: ");
    input(trip->destination.name, 50);

    printf("Nhap dia chi diem den moi: ");
    input(trip->destination.address, 100);

    printf("Nhap ngay gio chuyen xe moi (dd/mm/yy hh:mm): ");
    input_datetime(trip->date);

    while (1) {
        char buffer[10];
        printf("Nhap tong so ghe moi: ");
        input(buffer, 10);

        int seat = atoi(buffer);
        if (seat <= 0) printf("[Loi] So ghe phai > 0!\n");
        else {
            if (seat < trip->bookedSeats) {
                printf("[Loi] So ghe moi khong duoc nho hon so ghe da dat (%d)!\n", trip->bookedSeats);
            } else {
                trip->totalSeats = seat;
                break;
            }
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
    input(trip_id, 20);

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
    input(new_ticket.passenger.name, 50);

    printf("Nhap so dien thoai: ");
    input(new_ticket.passenger.phone, 15);

    while (1) {
        char buffer[10];
        printf("Nhap so ghe: ");
        input(buffer, 10);

        int seat = atoi(buffer);
        if (seat <= 0 || seat > trip->totalSeats) {
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
        input(buffer, 20);

        double price = atof(buffer);
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
    input(id, 20);

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

// ========================= HIEN THI THEO TRANG =========================

void display_trips_with_pagination(Trip trip_list[], int num_trips) {
    if (num_trips == 0) {
        printf("[Thong bao] Khong co chuyen xe!\n");
        return;
    }

    int pageSize;

    printf("Nhap page size (>0): ");
    pageSize = input_choice(1, 1000);

    int totalPages = (num_trips + pageSize - 1) / pageSize;

    while (1) {
        int page;
        printf("\nNhap so trang (1-%d), 0 de thoat: ", totalPages);
        page = input_choice(0, totalPages);

        if (page == 0) break;

        int start = (page - 1) * pageSize;
        int end = start + pageSize;
        if (end > num_trips) end = num_trips;

        printf("\n--- DANH SACH CHUYEN XE — Trang %d/%d ---\n", page, totalPages);

        for (int i = start; i < end; i++) {
            printf("%s | %s ? %s | %s | %d/%d\n",
                   trip_list[i].tripID,
                   trip_list[i].departure.name,
                   trip_list[i].destination.name,
                   trip_list[i].date,
                   trip_list[i].bookedSeats,
                   trip_list[i].totalSeats
            );
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
    input(id, 20);

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
    input(id, 20);

    for (int i = 0; i < num_tickets; i++) {
        if (strcmp(ticket_list[i].tiketId, id) == 0) {
            Ticket *tk = &ticket_list[i];

            printf("Chon trang thai moi (0: Binh thuong, 1: Khoa, 2: Huy): ");
            int newStatus = input_choice(0, 2);

            if (tk->status == STATUS_NORMAL && newStatus == STATUS_CANCELED) {
                // Cap nhat ghe da dat
                int trip_idx = check_ID(trip_list, num_trips, tk->tripId);
                if (trip_idx != -1) trip_list[trip_idx].bookedSeats--;
            }

            tk->status = newStatus;
            printf("[Thanh cong] Cap nhat trang thai ve %s thanh cong!\n", tk->tiketId);
            return;
        }
    }

    printf("[Loi] Khong tim thay ve!\n");
}

