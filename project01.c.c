#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 

#define MAX_TRIPS 50
#define MAX_TICKETS 100
typedef struct {
    char name [50]; // ten dia diem
    char address[100]; // dia chi cua dia diem
} Location;
typedef struct {
    char tripID[20]; // ma chuyen xe
    Location departure; // dia diem khoi hanh
    Location destination; // dia diem den
    char date[20]; // ngay gio chuyen xe
    int totalSeats; // tong so ghe
    int BookedSeats; // so ghe da dat
} Trip;
typedef struct {
    char name[50]; // ten hanh khach
    char phone[15]; // so dien thoai
} Passenger;
typedef struct {
    char tiketId[20]; // ma ve
    char TripId[20]; // ma chuyen xe
    Passenger passenger; // thong tin hanh khach
    int seatNumber; // so ghe da dat
    double price; // gia ve
    int paymentStatus; // trang thai thanh toan (0: chua thanh toan || 1: da thanh toan)
    char date[20]; // ngay gio dat ve
} Ticket;
void clear_input(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
void input(char dest[], int max_len){
    if (fgets(dest, max_len, stdin) != NULL){
        // tim va thay the ky tu newline ('\n') bang null terminator ('\0')
        dest[strcspn(dest, "\n")] = 0;
    }
}
void printMenu(){
    printf ("\n+------------------------------------------------------------+\n");
    printf ("|              ?? menu quan ly chuyen xe va ve ???               |\n");
    printf ("+------------------------------------------------------------+\n");
    printf ("|%-60s|\n", "1. them chuyen xe moi");
    printf ("|%-60s|\n", "2. cap nhat thong tin chuyen xe");
    printf ("|%-60s|\n", "3. dat ve");
    printf ("|%-60s|\n", "4. kiem tra tinh trang ve");
    printf ("|%-60s|\n", "5. liet ke chuyen xe");
    printf ("|%-60s|\n", "6. thanh toan ve");
    printf ("|%-60s|\n", "7. quan ly trang thai ve");
    printf ("|%-60s|\n", "8. bao cao thong ke & doanh thu");
    printf ("+------------------------------------------------------------+\n");
    printf ("|%-60s|\n", "0. thoat chuong trinh");
    printf ("+------------------------------------------------------------+\n");
}
int check_ID(const Trip trip_list[], int num_trip, const char *id){ 
    for (int i = 0; i < num_trip; i++){
        // so sanh chuoi (strcmp tra ve 0 neu hai chuoi bang nhau)
        if (strcmp(trip_list[i].tripID, id) == 0){
            return i; // tra ve chi so (index) neu id duoc tim thay
        }
    }
    return -1; // tra ve -1 neu id khong ton tai
}

void printf_display_trips(const Trip trip_list[], int num_trips){
    if (num_trips == 0){
        printf ("[thong bao] danh sach chuyen xe hien tai rong.\n");
        return; 
    }
    printf ("\n+-------------------------------------------------------------------------------------------------+\n"); //97
    printf ("|                                    danh sach chuyen xe                                          |\n");
    printf ("+-------------------------------------------------------------------------------------------------+\n"); 
    printf ("|%-10s| %-15s | %-15s | %-20s | %-7s | %-7s |\n", "id", "kh.hanh", "diem den", "thoi gian", "da dat", "tong");
    printf ("+-------------------------------------------------------------------------------------------------+\n"); 
    for (int i = 0; i < num_trips; i++){
        printf ("|%-10s| %-15s | %-15s | %-20s | %-7d | %-7d |\n", 
                trip_list[i].tripID,
                trip_list[i].departure.name,
                trip_list[i].destination.name,
                trip_list[i].date,
                trip_list[i].BookedSeats, 
                trip_list[i].totalSeats); 
    }
    printf ("+-------------------------------------------------------------------------------------------------+\n"); 
}
void addBusTrip(Trip trip_List[], int *num_trips){
    if (*num_trips >= MAX_TRIPS){
        printf ("[loi] danh sach chuyen xe da day, khong the them chuyen xe.\n");
        return;
    }

    Trip new_trip;
    char temp_id[20]; 

    printf ("\n---them chuyen xe moi---\n");

    do{
        printf ("nhap ma chuyen xe: ");
        input (temp_id, 20);
        // kiem tra id: neu check_id tra ve >= 0 (tim thay) thi la loi
        if (check_ID(trip_List, *num_trips, temp_id) != -1){
            printf ("[loi] ma chuyen xe '%s' da ton tai. vui long nhap lai!!!\n", temp_id);
        } else if (strlen(temp_id) == 0) {
            printf ("[loi] ma chuyen xe khong duoc de trong. vui long nhap lai!!!\n");
        } else {
            break; // thoat neu id hop le va khong trung
        }
    } while (true);
    strcpy(new_trip.tripID, temp_id);

    printf ("nhap ten dia diem khoi hanh: ");
    input(new_trip.departure.name, 50);

    printf ("nhap dia chi khoi hanh: ");
    input(new_trip.departure.address, 100);

    printf ("nhap ten dia diem den: ");
    input(new_trip.destination.name, 50);

    printf ("nhap dia chi den: ");
    input(new_trip.destination.address, 100);

    printf ("nhap ngay gio chuyen xe (vd: dd-mm-yyyy 00:00): ");
    input(new_trip.date, 20);

    do{
        printf ("nhap tong so ghe: ");
        // kiem tra dau vao la so nguyen duong
        if (scanf("%d", &new_trip.totalSeats) != 1 || new_trip.totalSeats <= 0){
            printf ("[loi] vui long nhap so ghe hop le (> 0) !!!\n");
            clear_input();
            continue;
        }
        clear_input(); // xoa bo dem sau khi nhap so
        break;
    } while (true);

    new_trip.BookedSeats = 0;
    trip_List[*num_trips] = new_trip;
    (*num_trips)++;

    printf ("[thanh cong] da them chuyen xe: %s -> %s (ma: %s) voi %d ghe.\n", 
            new_trip.departure.name, new_trip.destination.name, new_trip.tripID, new_trip.totalSeats);
} 
void update_bus_trip (Trip trip_list[], int num_trips){
    if (num_trips == 0){
        printf ("[loi] danh sach chuyen xe hien tai rong, khong the cap nhat !!!\n");
        return;
    }

    char id_update[20];
    printf ("\n---cap nhat thong tin chuyen xe---\n");
    printf ("nhap ma chuyen xe can cap nhat: ");
    input(id_update, 20);

    // tim chi so cua chuyen xe can cap nhat
    int index = check_ID(trip_list, num_trips, id_update);
    
    if(index == -1){ // neu khong tim thay
        printf ("[loi] khong tim thay chuyen xe voi ma id: %s .\n", id_update);
        return;
    }

    Trip *trip = &trip_list[index]; // con tro den chuyen xe

    printf ("* dang cap nhat cho chuyen xe: %s -> %s *\n", trip->departure.name, trip->destination.name);

    printf ("nhap ten dia diem khoi hanh moi (hien tai: %s): ", trip->departure.name);
    input(trip->departure.name, 50);

    printf ("nhap dia chi khoi hanh moi (hien tai: %s): ", trip->departure.address);
    input(trip->departure.address, 100);

    printf ("nhap ten dia diem den moi (hien tai: %s): ", trip->destination.name);
    input(trip->destination.name, 50);

    printf ("nhap dia chi den moi (hien tai: %s): ", trip->destination.address);
    input(trip->destination.address, 100);

    printf ("nhap vao ngay gio chuyen xe moi (hien tai: %s): ", trip->date);
    input (trip->date, 20);

    int new_totalSeats; 
    do{
        printf ("nhap vao tong so ghe moi (hien tai: %d | da dat: %d): ", trip->totalSeats, trip->BookedSeats);
        // kiem tra so ghe moi phai lon hon so ghe da dat
        if(scanf("%d", &new_totalSeats) != 1 || new_totalSeats < trip->BookedSeats){
            printf ("[loi] tong so ghe moi phai lon hon hoac bang so ghe da dat (%d)!!!\n", trip->BookedSeats);
            clear_input();
            continue;
        }
        clear_input();
        break;
    } while (true);

    trip->totalSeats = new_totalSeats;
    printf ("[thanh cong] da cap nhat thong tin chuyen xe %s thanh cong !!!\n", trip->tripID);
}
int main(){
    Trip trip_list[MAX_TRIPS]; // mang luu tru thong tin chuyen xe
    int num_trips = 0; // so luong chuyen xe hien tai
    int choice; // lua chon chuc nang

    do{
        printMenu();

        // vong lap kiem tra dau vao lua chon
        do{
            printf ("\nmoi ban nhap vao lua chon (0-8): ");
            // kiem tra nhap dung dinh dang va trong khoang
            if(scanf("%d", &choice) != 1 || choice < 0 || choice > 8){ 
                printf ("[loi] vui long nhap trong khoang 0-8 !!!\n");
                clear_input();
                continue;
            }
            clear_input();
            break;
        } while (true);
        switch(choice){
            case 0:
                printf("\n?? dang thoat chuong trinh quan ly chuyen xe... tam biet!\n");
                return 0; // thoat khoi main va ket thuc chuong trinh
            case 1:
                addBusTrip(trip_list, &num_trips);
                break;
            case 2:
                printf_display_trips(trip_list, num_trips); // hien thi truoc khi cap nhat
                update_bus_trip(trip_list, num_trips);
                printf_display_trips(trip_list, num_trips); // hien thi sau khi cap nhat
                break;
            case 5: // chuc nang liet ke
                printf_display_trips(trip_list, num_trips);
                break;
            // cac chuc nang chua trien khai
            case 3:
            case 4:
            case 6:
            case 7:
            case 8:
                printf("\n[thong bao] chuc nang %d chua duoc trien khai.\n", choice);
                break;
            default:
                break;	
        } 

    } while (true); // vong lap chinh cua chuong trinh

    return 0;
}
