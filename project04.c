#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TRIPS 50
#define MAX_TICKETS 100

typedef struct {
    char name[50]; 
    char address[100];
} Location;

typedef struct {
    char tripID[20];
    Location departure;
    Location destination;
    char date[20];
    int totalSeats;
    int bookedSeats;
} Trip;

typedef struct {
    char name[50];
    char phone[15];
} Passenger;

typedef struct {
    char tiketId[20];
    char tripId[20];
    Passenger passenger;
    int seatNumber;
    double price;
    int paymentStatus;  
    char date[20];
} Ticket;

void clear_input();
void input(char dest[], int max_len);
void printMenu();
int check_ID(Trip trip_list[], int num_trip, char *id);
void addBusTrip(Trip trip_List[], int *num_trips);
void update_bus_trip (Trip trip_list[], int num_trips);
void book_ticket(Trip trip_list[], int num_trips, Ticket ticket_list[], int *num_tickets);
void printf_display_trips(Trip trip_list[], int num_trips);
void check_ticket_status(Ticket ticket_list[], int num_tickets);  // CASE 4

int main(){
    Trip trip_list[MAX_TRIPS];
    Ticket ticket_list[MAX_TICKETS];
    int num_trips = 0;
    int num_tickets = 0;
    int choice;

    do {
        printMenu();

        do {
            printf("Moi ban nhap vao lua chon (1-8): ");
            if (scanf("%d", &choice) != 1 || choice < 1 || choice > 8){
                printf("[Loi] Lua chon phai tu 1-8!\n");
                clear_input();
                continue;
            }
            clear_input();
            break;
        } while (true);

        switch(choice){
            case 1:
                addBusTrip(trip_list, &num_trips);
                printf_display_trips(trip_list, num_trips);
                break;

            case 2:
                update_bus_trip(trip_list, num_trips);
                printf_display_trips(trip_list, num_trips);
                break;

            case 3:
                book_ticket(trip_list, num_trips, ticket_list, &num_tickets);
                break;

            case 4:    // ?? CASE 4 dã thêm
                check_ticket_status(ticket_list, num_tickets);
                break;

            default:
                printf("Chuc nang chua hoan thien.\n");
                break;
        }

    } while (true);

    return 0;
}

void clear_input(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void input(char dest[], int max_len){
    bool valid_input = false;
    do {
        if (fgets(dest, max_len, stdin) != NULL){
            dest[strcspn(dest, "\n")] = 0;

            if (strlen(dest) > 0){
                valid_input = true;
            } else {
                printf("[Loi] Khong duoc de trong. Nhap lai: ");
            }
        } else {
            break;
        }
    } while (!valid_input);
}

void printMenu(){
    printf("\n+------------------------------------------------------------+\n");
    printf("|                      MENU CHUC NANG                        |\n");
    printf("+------------------------------------------------------------+\n");
    printf("| 1. Them chuyen xe moi                                      |\n");
    printf("| 2. Cap nhat thong tin chuyen xe                            |\n");
    printf("| 3. Dat ve                                                  |\n");
    printf("| 4. Kiem tra tinh trang ve                                  |\n");
    printf("| 5. Liet ke chuyen xe                                       |\n");
    printf("| 6. Thanh toan ve                                           |\n");
    printf("| 7. Quan ly trang thai ve                                   |\n");
    printf("| 8. Bao cao thong ke & doanh thu                            |\n");
    printf("+------------------------------------------------------------+\n");
}

int check_ID(Trip trip_list[], int num_trip, char *id){
    for (int i = 0; i < num_trip; i++){
        if (strcmp(trip_list[i].tripID, id) == 0){
            return i;
        }
    }
    return -1;
}

void addBusTrip(Trip trip_List[], int *num_trips){
    if (*num_trips >= MAX_TRIPS){
        printf("[Loi] Danh sach chuyen xe da day.\n");
        return;
    }

    Trip new_trip;
    char temp_id[20];

    printf("\n---THEM CHUYEN XE MOI---\n");

    while (1){
        printf("Nhap Ma chuyen xe: ");
        input(temp_id, 20);
        if (check_ID(trip_List, *num_trips, temp_id) != -1){
            printf("[Loi] Ma chuyen xe da ton tai!\n");
        } else break;
    }
    strcpy(new_trip.tripID, temp_id);

    printf("Nhap Ten dia diem khoi hanh: ");
    input(new_trip.departure.name, 50);

    printf("Nhap dia chi khoi hanh: ");
    input(new_trip.departure.address, 100);

    printf("Nhap Ten dia diem den: ");
    input(new_trip.destination.name, 50);

    printf("Nhap Dia chi den: ");
    input(new_trip.destination.address, 100);

    printf("Nhap ngay gio chuyen xe: ");
    input(new_trip.date, 20);

    while (1){
        printf("Nhap tong so ghe: ");
        if (scanf("%d", &new_trip.totalSeats) != 1 || new_trip.totalSeats <= 0){
            printf("[Loi] So ghe phai > 0.\n");
            clear_input();
        } else break;
    }
    clear_input();

    new_trip.bookedSeats = 0;

    trip_List[*num_trips] = new_trip;
    (*num_trips)++;

    printf("[Thanh cong] Da them chuyen xe %s.\n", new_trip.tripID);
}

void update_bus_trip (Trip trip_list[], int num_trips){
    if (num_trips == 0){
        printf("[Loi] Danh sach chuyen xe rong.\n");
        return;
    }

    char id[20];
    printf("\n---CAP NHAT CHUYEN XE---\n");
    printf("Nhap Ma chuyen xe: ");
    input(id, 20);

    int index = check_ID(trip_list, num_trips, id);
    if (index == -1){
        printf("[Loi] Khong tim thay chuyen xe.\n");
        return;
    }

    Trip *trip = &trip_list[index];

    printf("Nhap Ten dia diem khoi hanh moi: ");
    input(trip->departure.name, 50);

    printf("Nhap Dia chi khoi hanh moi: ");
    input(trip->departure.address, 100);

    printf("Nhap Ten dia diem den moi: ");
    input(trip->destination.name, 50);

    printf("Nhap Dia chi den moi: ");
    input(trip->destination.address, 100);

    printf("Nhap ngay gio chuyen xe moi: ");
    input(trip->date, 20);

    while (1){
        printf("Nhap tong so ghe moi: ");
        if (scanf("%d", &trip->totalSeats) != 1 || trip->totalSeats <= 0){
            printf("[Loi] So ghe phai > 0.\n");
            clear_input();
        } else break;
    }
    clear_input();

    printf("[Thanh cong] Da cap nhat chuyen xe.\n");
}

void book_ticket(Trip trip_list[], int num_trips, Ticket ticket_list[], int *num_tickets){
    if (num_trips == 0){
        printf("[Loi] Khong co chuyen xe nao!\n");
        return;
    }
    if (*num_tickets >= MAX_TICKETS){
        printf("[Loi] He thong da het ve!\n");
        return;
    }

    char trip_id[20];
    printf("\n---DAT VE---\n");
    printf("Nhap Ma chuyen xe: ");
    input(trip_id, 20);

    int idx = check_ID(trip_list, num_trips, trip_id);
    if (idx == -1){
        printf("[Loi] Khong tim thay chuyen xe.\n");
        return;
    }

    Trip *trip = &trip_list[idx];

    if (trip->bookedSeats >= trip->totalSeats){
        printf("[Loi] Chuyen xe da het ghe.\n");
        return;
    }

    Ticket new_ticket;
    strcpy(new_ticket.tripId, trip_id);

    printf("Nhap ten hanh khach: ");
    input(new_ticket.passenger.name, 50);

    printf("Nhap so dien thoai: ");
    while (1){
        input(new_ticket.passenger.phone, 15);
        bool digit = true;
        for (int i = 0; new_ticket.passenger.phone[i]; i++){
            if (new_ticket.passenger.phone[i] < '0' || new_ticket.passenger.phone[i] > '9'){
                digit = false;
                break;
            }
        }
        if (!digit) {
            printf("[Loi] SDT chi duoc chua so. Nhap lai: ");
        } else break;
    }

    int seat;
    printf("Nhap so ghe muon dat: ");
    while (1){
        if (scanf("%d", &seat) != 1 || seat <= 0 || seat > trip->totalSeats){
            printf("[Loi] So ghe phai tu 1-%d. Nhap lai: ", trip->totalSeats);
            clear_input();
            continue;
        }
        clear_input();

        bool taken = false;
        for (int i = 0; i < *num_tickets; i++){
            if (strcmp(ticket_list[i].tripId, trip_id) == 0 &&
                ticket_list[i].seatNumber == seat){
                taken = true;
                break;
            }
        }
        if (taken){
            printf("[Loi] Ghe nay da duoc dat. Chon ghe khac: ");
        } else break;
    }
    new_ticket.seatNumber = seat;

    printf("Nhap gia ve: ");
    while (1){
        if (scanf("%lf", &new_ticket.price) != 1 || new_ticket.price <= 0){
            printf("[Loi] Gia ve phai > 0. Nhap lai: ");
            clear_input();
        } else break;
    }
    clear_input();

    printf("Nhap ngay dat ve: ");
    input(new_ticket.date, 20);

    sprintf(new_ticket.tiketId, "V%03d", *num_tickets + 1);
    new_ticket.paymentStatus = 0;

    ticket_list[*num_tickets] = new_ticket;
    (*num_tickets)++;

    trip->bookedSeats++;

    printf("\n[Thanh cong] Dat ve thanh cong!\n");
    printf("Ma ve: %s | Ghe %d | Gia %.2lf\n",
           new_ticket.tiketId, new_ticket.seatNumber, new_ticket.price);
}

void check_ticket_status(Ticket ticket_list[], int num_tickets){
    if (num_tickets == 0){
        printf("[Thong bao] Chua co ve nao trong he thong.\n");
        return;
    }

    char id[20];
    printf("\n---KIEM TRA TINH TRANG VE---\n");
    printf("Nhap ma ve: ");
    input(id, 20);

    if (strlen(id) == 0){
        printf("[Loi] Ma ve khong duoc de trong.\n");
        return;
    }

    int found = -1;
    for (int i = 0; i < num_tickets; i++){
        if (strcmp(ticket_list[i].tiketId, id) == 0){
            found = i;
            break;
        }
    }

    if (found == -1){
        printf("[Loi] Khong tim thay ve.\n");
        return;
    }

    Ticket *tk = &ticket_list[found];

    printf("\n---THONG TIN VE---\n");
    printf("Ma ve: %s\n", tk->tiketId);
    printf("Ten hanh khach: %s\n", tk->passenger.name);
    printf("So dien thoai: %s\n", tk->passenger.phone);
    printf("Ma chuyen xe: %s\n", tk->tripId);
    printf("So ghe: %d\n", tk->seatNumber);
    printf("Gia ve: %.2lf\n", tk->price);
    printf("Trang thai thanh toan: %s\n", tk->paymentStatus == 0 ? "Chua thanh toan" : "Da thanh toan");
    printf("Ngay dat ve: %s\n", tk->date);
}

void printf_display_trips(Trip trip_list[], int num_trips){
    if (num_trips == 0){
        printf("[Thong bao] Khong co chuyen xe nao.\n");
        return;
    }

    printf("\n+----------------------------------------------------------------------------------------------+\n");
    printf("|                                    DANH SACH CHUYEN XE                                       |\n");
    printf("+----------------------------------------------------------------------------------------------+\n");
    printf("|%-10s| %-15s | %-15s | %-20s | %-10s | %-10s|\n",
           "ID", "KH.HANH", "DIEM DEN", "THOI GIAN", "DA DAT", "TONG");
    printf("+----------------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < num_trips; i++){
        printf("|%-10s| %-15s | %-15s | %-20s | %-10d | %-10d|\n",
               trip_list[i].tripID,
               trip_list[i].departure.name,
               trip_list[i].destination.name,
               trip_list[i].date,
               trip_list[i].bookedSeats,
               trip_list[i].totalSeats);
    }
    printf("+----------------------------------------------------------------------------------------------+\n");
}

