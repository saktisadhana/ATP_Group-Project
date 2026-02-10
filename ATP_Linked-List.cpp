#include <iostream>
#include <string>
using namespace std;

// "data" dalam node adalah struct ini
struct studentData 
{
    long long NRP; //long long karena format NRP ITS yang melebihi batas untuk int dll
    string namaLengkap; 
    long long noTelpon; // diganti ke long long agar muat nomor telepon
};

// struct untuk node
struct Node 
{
    studentData data; // data = studentData 
    Node* next;
};

// fungsi untuk embuat node baru
Node* createNode(studentData data) 
{
    Node* newNode = new Node;
    newNode->data = data;
    newNode->next = nullptr;
    return newNode;
}

// insert at the beggining
void inBegin(Node*& head, studentData value) 
{
    Node* newNode = createNode(value);
    newNode->next = head;
    head = newNode;
}

// insert at the end
void inEnd(Node*& head, studentData value) 
{
    Node* newNode = createNode(value);
    //check jika list masih kosong
    if (head == nullptr) {
        head = newNode; //maka head akan menunjuk ke node baru tersebut
    } else {
        // Cari node terakhir
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        // Sambungkan node terakhir ke node baru
        current->next = newNode;
    }
}

// insert at certain index
void inIndex(Node*& head, studentData value, int index)
{
    //  buat node baru berisi data mahasiswa
    Node* newNode = createNode(value);
    // jika index = 0, sama seperti insert di awal
    if (index == 0) {
        newNode->next = head;
        head = newNode;
        cout << "~>Data berhasil ditambahkan di index 0 (awal).\n";
        return;
    }
    Node* current = head;
    int count = 0;
    // Traversing untuk mencari posisi sebelum index tujuan
    while (current != nullptr && count < index - 1) {
        current = current->next;
        count++;
    }

    // Jika index tidak valid (lebih besar dari jumlah node)
    if (current == nullptr) {
        cout << "~>Index tidak valid! Tidak dapat menambahkan data.\n";
        delete newNode; // Hapus node yang tidak terpakai
        return;
    }

    // Sisipkan node baru pada posisi index
    newNode->next = current->next;
    current->next = newNode;

    cout << "~>Data berhasil ditambahkan di index " << index << ".\n";
}

void delBegin(Node*& head) 
{
    if (head == nullptr) {
        cout << "~>Linklist kosong, tidak ada yang dihapus\n";
        return; 
    }
    Node* temp = head;
    head = head->next; 
    delete temp; 
    cout << "~>Data awal berhasil dihapus\n";
}   
// Delete At The End
void delEnd(Node*& head)
{
    // Jika list kosong
    if (head == nullptr) {
        cout << "List kosong, tidak ada data untuk dihapus.\n";
        return;
    }

    // Jika hanya ada satu node
    if (head->next == nullptr) {
        delete head;
        head = nullptr;
        cout << "Data terakhir berhasil dihapus\n";
        return;
    }

    // Jika ada lebih dari satu node
    Node* current = head; 
    Node* prev = nullptr;

    // Telusuri sampai node terakhir
    while (current->next != nullptr) {
        prev = current;
        current = current->next;
    }

    // prev adalah node sebelum terakhir, current adalah node terakhir
    delete current;
    prev->next = nullptr;
    cout << "Data terakhir berhasil dihapus\n";
}
// Delete At Certain Index
void delIndex(Node*& head, int index)
{
    // Jika list kosong
    if (head == nullptr) {
        cout << "~>List masih kosong!\n";
        return;
    }

    // Jika index = 0, berarti delete di awal
    if (index == 0) {
        Node* temp = head;
        head = head->next;
        delete temp;
        cout << "~>Data pada index 0 berhasil dihapus.\n";
        return;
    }
    Node* current = head;
    Node* prev = nullptr;
    int count = 0;
    // Traversing sampai posisi index
    while (current != nullptr && count < index) {
        prev = current;
        current = current->next;
        count++;
    }
    // Jika index tidak ditemukan (lebih besar dari jumlah node)
    if (current == nullptr) {
        cout << "~>Index tidak valid!\n";
        return;
    }
    // Hapus node pada posisi index
    prev->next = current->next;
    delete current;

    cout << "~>Data pada index " << index << " berhasil dihapus.\n";
}
// print linked list
void printList(Node* head) {
    
    cout << "\n~~~___--- Output Data ---___~~~\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    if (head == nullptr) { // jika list masih kosong
    cout << "   (List kosong)\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    return;
    }

    Node* current = head; //jika list tidak kosong, buat current menjadi head
    int index = 0; //set data perrtama sebagai index 0
    while (current != nullptr) { 
        if (index == 0) 
        {
        cout << "Data ke-" << index << ":" << "Head\n";  // untuk menandakan bahwa index 0 == head
        } else 
        cout << "Data ke-" << index << ":" << endl; // sisanya akan di print seperti ini
        cout << "  Nama  : " << current->data.namaLengkap << endl;
        cout << "  NRP   : " << current->data.NRP << endl;
        cout << "  No HP : " << current->data.noTelpon << endl;
        cout << "-------------------------------\n";
        
        current = current->next; //setelah selesai menunjukkan index x, current data bukan = head tetapi next(data selanjutnya)
        index++; // index menjadi x+1
    }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

// fungsi untuk input data
studentData inptData() {
    studentData data;
    
    cout << "  Masukkan Nama Lengkap: ";
    cin.ignore(10000, '\n'); // membersihkan *seluruh* sisa baris
    getline(cin, data.namaLengkap);
    
    cout << "  Masukkan NRP         : ";
    cin >> data.NRP;
    
    cout << "  Masukkan No. Telepon : ";
    cin >> data.noTelpon;
    return data;
}

int main() {
    Node* head = nullptr; // list dimulai sebagai kosong
    int opsiMain; int opsiInsert; int opsiDelete; // variabel unntuk menampung opsi
    do {
        cout << "\n~~~___---@@ Welcome @@---___~~~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "~ Pilih salah satu opsi!       ~\n";
        cout << "~> (1) Add Data                ~\n";
        cout << "~> (2) Delete Data             ~\n";
        cout << "~> (3) View Data               ~\n";
        cout << "~> (0) Keluar                  ~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "~> Input Opsi = "; cin >> opsiMain;
        studentData dataBaru;
        switch (opsiMain) 
        {
            case 1:
                cout << "\n~~~___--- Input Data! ---___~~~\n"; 
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                cout << "~~~~!Pilih salah satu opsi!~~~~\n";
                cout << "~>(1)Insert At The Beginning  ~\n";
                cout << "~>(2)Insert At The End        ~\n";
                cout << "~>(3)Insert At Specified Index~\n";
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                cout << "~>Input Opsi = "; cin >> opsiInsert;
            switch (opsiInsert)
            {
                case 1:
                    dataBaru = inptData();
                    inBegin(head, dataBaru);
                    cout << "\n~>Data berhasil ditambahkan di depan\n";
                    break;
                case 2:
                    dataBaru = inptData();
                    inEnd(head, dataBaru);
                    cout << "\n~>Data berhasil ditambahkan di belakang\n";
                    break;
                case 3:
                    dataBaru = inptData(); int index; printList(head); 
                    cout << "~>Input Index = "; cin >> index;
                    inIndex(head, dataBaru, index); 
                    break;
                default:
                    cout << "\n~>Opsi tidak valid! Silakan pilih 0-3.\n";
                    break;
            }
                break;
            case 2:
                cout << "\n~~~___--- Delete Data ---___~~~\n"; 
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                cout << "~~~~!Pilih salah satu opsi!~~~~\n";
                cout << "~>(1)Delete At The Beginning  ~\n";
                cout << "~>(2)Delete At The End        ~\n";
                cout << "~>(3)Delete At Specified Index~\n";
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                cout << "~>Input Opsi = "; cin >> opsiDelete;
                switch (opsiDelete)
                {
                case 1:
                delBegin(head);
                    break;
                case 2:
                delEnd(head);
                    break;
                case 3:
                int index;
                cout << "~>Input Index = "; cin >> index;
                delIndex(head,index);
                    break;
                default:
                    cout << "\n~>Opsi tidak valid! Silakan pilih 0-3.\n";
                break;
                }
                break;
            case 3:
                printList(head);
                break;
            case 0:
                cout << "\nTerima kasih! Program selesai.\n";
                break;
            default:
                cout << "\nOpsi tidak valid! Silakan pilih 0-3.\n";
                break;
        }
    } while (opsiMain != 0);
    // hapus semua node untuk menghindari memory leak
    Node* current = head;
    while (current != nullptr) 
    {
        Node* temp = current;
        current = current->next;
        delete temp;
    }

    return 0;
}
