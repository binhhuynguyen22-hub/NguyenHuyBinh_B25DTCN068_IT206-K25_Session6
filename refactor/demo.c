#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define MAX_INPUT 50

// ==================== ENUMS & CONSTANTS ====================
typedef enum {
    ROOM_AVAILABLE = 0,
    ROOM_OCCUPIED = 1,
    ROOM_MAINTENANCE = 2
} RoomStatus;

typedef enum {
    ROOM_SINGLE = 1,
    ROOM_DOUBLE = 2
} RoomType;

// ==================== STRUCTURES ====================
typedef struct {
    char roomId[10];
    int type;
    double price;
    int status;
} Room;

typedef struct {
    char bookId[20];
    char roomId[10];
    char customerName[50];
    char checkInDate[11];
    int days;
    double totalCost;
} Booking;

typedef struct {
    Room rooms[MAX];
    int count;
} RoomList;

typedef struct {
    Booking bookings[MAX];
    int count;
} BookingList;

// ==================== FUNCTION PROTOTYPES ====================
// Menu & UI
void displayMainMenu();
void pauseScreen();
void clearScreen();

// Input Validation
int isValidInteger(const char *str);
int isValidDouble(const char *str);
int isValidDate(const char *date);
int readInteger(const char *prompt, int min, int max);
double readDouble(const char *prompt, double min);
void readString(const char *prompt, char *buffer, int size);

// Room Management
void initializeRooms(RoomList *roomList);
void addRoom(RoomList *roomList);
void updateRoom(RoomList *roomList);
void maintenanceRoom(RoomList *roomList);
void displayRooms(const RoomList *roomList);
void searchRoomByType(const RoomList *roomList);
void sortRoomByPrice(RoomList *roomList);
int findRoomById(const RoomList *roomList, const char *roomId);
int isRoomIdExists(const RoomList *roomList, const char *roomId);

// Booking Management
void checkIn(RoomList *roomList, BookingList *bookingList);
void displayRentalHistory(const BookingList *bookingList);
void generateBookingId(char *bookId, const char *roomId);
void printInvoice(const Booking *booking, const Room *room);

// Utility Functions
const char* getRoomTypeName(int type);
const char* getRoomStatusName(int status);
void printTableHeader();
void printTableRow(const Room *room);
void printTableFooter();

// ==================== MAIN ====================
int main() {
    RoomList roomList = {.count = 0};
    BookingList bookingList = {.count = 0};
    
    initializeRooms(&roomList);
    
    int option;
    do {
        displayMainMenu();
        option = readInteger("Moi ban nhap lua chon: ", 1, 9);
        clearScreen();
        
        switch(option) {
            case 1: addRoom(&roomList); break;
            case 2: updateRoom(&roomList); break;
            case 3: maintenanceRoom(&roomList); break;
            case 4: displayRooms(&roomList); break;
            case 5: searchRoomByType(&roomList); break;
            case 6: sortRoomByPrice(&roomList); break;
            case 7: checkIn(&roomList, &bookingList); break;
            case 8: displayRentalHistory(&bookingList); break;
            case 9: 
                printf("Cam on da su dung, hen gap lai!\n");
                break;
        }
        
        if(option != 9) pauseScreen();
        
    } while(option != 9);
    
    return 0;
}

// ==================== MENU & UI ====================
void displayMainMenu() {
    printf("----------------------------------------------\n");
    printf("|           QUAN LY KHACH SAN                |\n");
    printf("----------------------------------------------\n");
    printf("|1. Them phong moi                           |\n");
    printf("|2. Cap nhat phong                           |\n");
    printf("|3. Bao tri phong (khoa)                     |\n");
    printf("|4. Hien thi danh sach                       |\n");
    printf("|5. Tim kiem theo loai                       |\n");
    printf("|6. Sap xep theo gia                         |\n");
    printf("|7. Check-in (giao dich)                     |\n");
    printf("|8. Lich su thue phong                       |\n");
    printf("|9. Thoat                                    |\n");
    printf("----------------------------------------------\n");
}

void pauseScreen() {
    printf("\nNhap Enter de tiep tuc...");
    while(getchar() != '\n');
    getchar();
}

void clearScreen() {
    system("cls");
}

// ==================== INPUT VALIDATION ====================
int isValidInteger(const char *str) {
    if(strlen(str) == 0) return 0;
    
    int start = (str[0] == '-' || str[0] == '+') ? 1 : 0;
    if(strlen(str) == start) return 0;
    
    for(int i = start; i < strlen(str); i++) {
        if(!isdigit(str[i])) return 0;
    }
    return 1;
}

int isValidDouble(const char *str) {
    if(strlen(str) == 0) return 0;
    
    int dotCount = 0;
    int start = (str[0] == '-' || str[0] == '+') ? 1 : 0;
    if(strlen(str) == start) return 0;
    
    for(int i = start; i < strlen(str); i++) {
        if(str[i] == '.') {
            if(++dotCount > 1) return 0;
        } else if(!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int isValidDate(const char *date) {
    if(strlen(date) != 10 || date[2] != '/' || date[5] != '/') return 0;
    
    for(int i = 0; i < 10; i++) {
        if(i != 2 && i != 5 && !isdigit(date[i])) return 0;
    }
    
    int day = atoi(date);
    int month = atoi(date + 3);
    int year = atoi(date + 6);
    
    if(day < 1 || day > 31 || month < 1 || month > 12) return 0;
    if(year < 1900 || year > 2100) return 0;
    
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }
    
    return (day <= daysInMonth[month]);
}

int readInteger(const char *prompt, int min, int max) {
    char buffer[MAX_INPUT];
    int number;
    
    while(1) {
        printf("%s", prompt);
        if(!fgets(buffer, sizeof(buffer), stdin)) continue;
        
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if(strlen(buffer) == 0) {
            printf("Loi: Khong duoc de trong!\n");
            continue;
        }
        
        if(!isValidInteger(buffer)) {
            printf("Loi: Chi duoc nhap so nguyen!\n");
            continue;
        }
        
        number = atoi(buffer);
        
        if(number < min || number > max) {
            printf("Loi: Gia tri phai tu %d den %d!\n", min, max);
            continue;
        }
        
        return number;
    }
}

double readDouble(const char *prompt, double min) {
    char buffer[MAX_INPUT];
    double number;
    
    while(1) {
        printf("%s", prompt);
        if(!fgets(buffer, sizeof(buffer), stdin)) continue;
        
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if(strlen(buffer) == 0) {
            printf("Loi: Khong duoc de trong!\n");
            continue;
        }
        
        if(!isValidDouble(buffer)) {
            printf("Loi: Chi duoc nhap so thuc!\n");
            continue;
        }
        
        number = atof(buffer);
        
        if(number < min) {
            printf("Loi: Gia tri phai lon hon %.2lf!\n", min);
            continue;
        }
        
        return number;
    }
}

void readString(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// ==================== ROOM MANAGEMENT ====================
void initializeRooms(RoomList *roomList) {
    Room defaultRooms[] = {
        {"101", ROOM_SINGLE, 150000, ROOM_AVAILABLE},
        {"102", ROOM_DOUBLE, 500000, ROOM_AVAILABLE},
        {"103", ROOM_SINGLE, 280000, ROOM_AVAILABLE},
        {"104", ROOM_DOUBLE, 550000, ROOM_AVAILABLE},
        {"105", ROOM_SINGLE, 320000, ROOM_AVAILABLE},
        {"106", ROOM_DOUBLE, 600000, ROOM_AVAILABLE},
        {"107", ROOM_SINGLE, 290000, ROOM_AVAILABLE}
    };
    
    int count = sizeof(defaultRooms) / sizeof(defaultRooms[0]);
    for(int i = 0; i < count; i++) {
        roomList->rooms[i] = defaultRooms[i];
    }
    roomList->count = count;
}

int findRoomById(const RoomList *roomList, const char *roomId) {
    for(int i = 0; i < roomList->count; i++) {
        if(strcmp(roomList->rooms[i].roomId, roomId) == 0) {
            return i;
        }
    }
    return -1;
}

int isRoomIdExists(const RoomList *roomList, const char *roomId) {
    return findRoomById(roomList, roomId) != -1;
}

void addRoom(RoomList *roomList) {
    if(roomList->count >= MAX) {
        printf("Loi: Da day, khong the them phong!\n");
        return;
    }
    
    printf("\n===== THEM PHONG MOI =====\n");
    
    Room newRoom;
    
    // Input room ID with validation
    while(1) {
        readString("Nhap ma phong: ", newRoom.roomId, sizeof(newRoom.roomId));
        
        if(strlen(newRoom.roomId) == 0) {
            printf("Loi: Ma phong khong duoc rong!\n");
            continue;
        }
        
        if(isRoomIdExists(roomList, newRoom.roomId)) {
            printf("Loi: Phong %s da ton tai!\n", newRoom.roomId);
            continue;
        }
        
        break;
    }
    
    newRoom.type = readInteger("Nhap loai phong (1: Don, 2: Doi): ", 1, 2);
    newRoom.price = readDouble("Nhap gia phong: ", 0.01);
    newRoom.status = ROOM_AVAILABLE;
    
    roomList->rooms[roomList->count++] = newRoom;
    
    printf("\nThem phong %s thanh cong!\n", newRoom.roomId);
}

void updateRoom(RoomList *roomList) {
    printf("\n===== CAP NHAT PHONG =====\n");
    
    char roomId[10];
    readString("Nhap ma phong can cap nhat: ", roomId, sizeof(roomId));
    
    int index = findRoomById(roomList, roomId);
    
    if(index == -1) {
        printf("Loi: Khong tim thay phong %s!\n", roomId);
        return;
    }
    
    Room *room = &roomList->rooms[index];
    
    if(room->status == ROOM_OCCUPIED) {
        printf("Loi: Phong dang co khach, khong the cap nhat!\n");
        return;
    }
    
    printf("\nThong tin hien tai:\n");
    printf("Ma phong: %s\n", room->roomId);
    printf("Loai: %s\n", getRoomTypeName(room->type));
    printf("Gia: %.2lf VND\n", room->price);
    printf("Trang thai: %s\n", getRoomStatusName(room->status));
    
    printf("\n--- Nhap thong tin moi ---\n");
    room->type = readInteger("Loai phong (1: Don, 2: Doi): ", 1, 2);
    room->price = readDouble("Gia phong: ", 0.01);
    
    printf("\nCap nhat phong %s thanh cong!\n", room->roomId);
}

void maintenanceRoom(RoomList *roomList) {
    if(roomList->count == 0) {
        printf("Loi: Khong co phong trong he thong!\n");
        return;
    }
    
    printf("\n===== BAO TRI PHONG =====\n");
    
    char roomId[10];
    readString("Nhap ma phong can bao tri: ", roomId, sizeof(roomId));
    
    int index = findRoomById(roomList, roomId);
    
    if(index == -1) {
        printf("Loi: Khong tim thay phong %s!\n", roomId);
        return;
    }
    
    Room *room = &roomList->rooms[index];
    
    if(room->status == ROOM_OCCUPIED) {
        printf("Loi: Phong dang co khach, khong the bao tri!\n");
        return;
    }
    
    room->status = ROOM_MAINTENANCE;
    printf("Phong %s da chuyen sang trang thai bao tri!\n", room->roomId);
}

void displayRooms(const RoomList *roomList) {
    if(roomList->count == 0) {
        printf("Khong co phong trong danh sach!\n");
        return;
    }
    
    const int ITEMS_PER_PAGE = 10;
    int totalPages = (roomList->count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    int currentPage = 0;
    
    while(1) {
        clearScreen();
        printf("===== DANH SACH PHONG (Trang %d/%d) =====\n\n", 
               currentPage + 1, totalPages);
        
        printTableHeader();
        
        int start = currentPage * ITEMS_PER_PAGE;
        int end = start + ITEMS_PER_PAGE;
        if(end > roomList->count) end = roomList->count;
        
        for(int i = start; i < end; i++) {
            printTableRow(&roomList->rooms[i]);
        }
        
        printTableFooter();
        
        printf("\nNhap so trang (1-%d), Enter = trang sau, 'q' = thoat: ", totalPages);
        
        char input[10];
        if(!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = '\0';
        
        if(strlen(input) == 0) {
            if(currentPage < totalPages - 1) {
                currentPage++;
            } else {
                printf("\nDa den trang cuoi!\n");
                pauseScreen();
                break;
            }
        } else if(strcmp(input, "q") == 0) {
            break;
        } else if(isValidInteger(input)) {
            int page = atoi(input);
            if(page >= 1 && page <= totalPages) {
                currentPage = page - 1;
            } else {
                printf("So trang khong hop le!\n");
                pauseScreen();
            }
        }
    }
}

void searchRoomByType(const RoomList *roomList) {
    if(roomList->count == 0) {
        printf("Khong co phong de tim kiem!\n");
        return;
    }
    
    printf("\n===== TIM KIEM PHONG THEO LOAI =====\n");
    int type = readInteger("Nhap loai phong (1: Don, 2: Doi): ", 1, 2);
    
    printf("\nKet qua tim kiem:\n");
    printTableHeader();
    
    int found = 0;
    for(int i = 0; i < roomList->count; i++) {
        if(roomList->rooms[i].type == type) {
            printTableRow(&roomList->rooms[i]);
            found = 1;
        }
    }
    
    printTableFooter();
    
    if(!found) {
        printf("\nKhong tim thay phong phu hop!\n");
    }
}

void sortRoomByPrice(RoomList *roomList) {
    if(roomList->count == 0) {
        printf("Khong co phong de sap xep!\n");
        return;
    }
    
    // Bubble sort descending by price
    for(int i = 0; i < roomList->count - 1; i++) {
        for(int j = i + 1; j < roomList->count; j++) {
            if(roomList->rooms[i].price < roomList->rooms[j].price) {
                Room temp = roomList->rooms[i];
                roomList->rooms[i] = roomList->rooms[j];
                roomList->rooms[j] = temp;
            }
        }
    }
    
    printf("\n===== DANH SACH DA SAP XEP (GIAM DAN) =====\n");
    printTableHeader();
    
    for(int i = 0; i < roomList->count; i++) {
        printTableRow(&roomList->rooms[i]);
    }
    
    printTableFooter();
}

// ==================== BOOKING MANAGEMENT ====================
void generateBookingId(char *bookId, const char *roomId) {
    strcpy(bookId, "BK");
    strcat(bookId, roomId);
}

void checkIn(RoomList *roomList, BookingList *bookingList) {
    if(roomList->count == 0) {
        printf("Loi: Chua co phong trong he thong!\n");
        return;
    }
    
    printf("\n===== CHECK-IN =====\n");
    
    char roomId[10];
    readString("Nhap ma phong: ", roomId, sizeof(roomId));
    
    int index = findRoomById(roomList, roomId);
    
    if(index == -1) {
        printf("Loi: Khong tim thay phong %s!\n", roomId);
        return;
    }
    
    Room *room = &roomList->rooms[index];
    
    if(room->status != ROOM_AVAILABLE) {
        if(room->status == ROOM_OCCUPIED) {
            printf("Loi: Phong da co khach!\n");
        } else {
            printf("Loi: Phong dang bao tri!\n");
        }
        return;
    }
    
    Booking newBooking;
    
    readString("Nhap ten khach hang: ", newBooking.customerName, 
               sizeof(newBooking.customerName));
    
    while(1) {
        readString("Nhap ngay check-in (DD/MM/YYYY): ", newBooking.checkInDate, 
                   sizeof(newBooking.checkInDate));
        
        int year = atoi(newBooking.checkInDate + 6);
        
        if(year >= 2025 && isValidDate(newBooking.checkInDate)) {
            break;
        }
        
        printf("Loi: Ngay khong hop le! Vui long nhap lai.\n");
    }
    
    newBooking.days = readInteger("Nhap so ngay thue (1-365): ", 1, 365);
    newBooking.totalCost = room->price * newBooking.days;
    
    generateBookingId(newBooking.bookId, room->roomId);
    strcpy(newBooking.roomId, room->roomId);
    
    room->status = ROOM_OCCUPIED;
    
    bookingList->bookings[bookingList->count++] = newBooking;
    
    printf("\nCheck-in thanh cong!\n");
    printInvoice(&newBooking, room);
}

void displayRentalHistory(const BookingList *bookingList) {
    if(bookingList->count == 0) {
        printf("Chua co giao dich nao!\n");
        return;
    }
    
    printf("\n===== LICH SU THUE PHONG =====\n");
    
    char roomId[10];
    readString("Nhap ma phong: ", roomId, sizeof(roomId));
    
    int found = 0;
    for(int i = 0; i < bookingList->count; i++) {
        if(strcmp(bookingList->bookings[i].roomId, roomId) == 0) {
            found = 1;
            break;
        }
    }
    
    if(!found) {
        printf("Phong %s chua co lich su thue!\n", roomId);
        return;
    }
    
    printf("\n");
    for(int i = 0; i < 100; i++) printf("=");
    printf("\n|%-5s|%-12s|%-8s|%-20s|%-12s|%-8s|%-15s|\n",
           "STT", "Ma booking", "Phong", "Khach hang", "Ngay", "So ngay", "Tong tien");
    for(int i = 0; i < 100; i++) printf("-");
    printf("\n");
    
    int stt = 1;
    for(int i = 0; i < bookingList->count; i++) {
        if(strcmp(bookingList->bookings[i].roomId, roomId) == 0) {
            Booking *b = &bookingList->bookings[i];
            printf("|%-5d|%-12s|%-8s|%-20s|%-12s|%-8d|%15.2lf|\n",
                   stt++, b->bookId, b->roomId, b->customerName,
                   b->checkInDate, b->days, b->totalCost);
        }
    }
    
    for(int i = 0; i < 100; i++) printf("=");
    printf("\n");
}

void printInvoice(const Booking *booking, const Room *room) {
    printf("\n");
    for(int i = 0; i < 40; i++) printf("=");
    printf("\n");
    printf("         HOA DON CHECK-IN\n");
    for(int i = 0; i < 40; i++) printf("=");
    printf("\n");
    printf("Ma booking  : %s\n", booking->bookId);
    printf("So phong    : %s\n", room->roomId);
    printf("Loai phong  : %s\n", getRoomTypeName(room->type));
    printf("Khach hang  : %s\n", booking->customerName);
    printf("Ngay nhan   : %s\n", booking->checkInDate);
    printf("So ngay     : %d\n", booking->days);
    printf("Don gia     : %.2lf VND\n", room->price);
    for(int i = 0; i < 40; i++) printf("-");
    printf("\n");
    printf("TONG TIEN   : %.2lf VND\n", booking->totalCost);
    for(int i = 0; i < 40; i++) printf("=");
    printf("\n");
}

// ==================== UTILITY FUNCTIONS ====================
const char* getRoomTypeName(int type) {
    return (type == ROOM_SINGLE) ? "Don" : "Doi";
}

const char* getRoomStatusName(int status) {
    switch(status) {
        case ROOM_AVAILABLE: return "Trong";
        case ROOM_OCCUPIED: return "Dang thue";
        case ROOM_MAINTENANCE: return "Bao tri";
        default: return "Khong xac dinh";
    }
}

void printTableHeader() {
    for(int i = 0; i < 70; i++) printf("=");
    printf("\n|%-8s|%-15s|%-20s|%-20s|\n", 
           "Ma phong", "Loai", "Gia (VND)", "Trang thai");
    for(int i = 0; i < 70; i++) printf("-");
    printf("\n");
}

void printTableRow(const Room *room) {
    printf("|%-8s|%-15s|%20.2lf|%-20s|\n",
           room->roomId,
           getRoomTypeName(room->type),
           room->price,
           getRoomStatusName(room->status));
}

void printTableFooter() {
    for(int i = 0; i < 70; i++) printf("=");
    printf("\n");
}
