#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

// ======================= KONSTAN ========================== batas ukuran array
#define MAX_JUDUL 100
#define MAX_ARTIST 100
#define MAX_GENRE 50
#define MAX_LIRIK 5000
#define MAX_LIRIK_DB 50
#define MAX_HISTORY 10

// =================== STRUCT DATA ========================== untuk beberapa tipe data/fungsi

struct Lagu
{
    char judul[MAX_JUDUL];
    char artist[MAX_ARTIST];
    char genre[MAX_GENRE];
    bool favorit;
    Lagu *next;
};

struct PlaylistLaguNode
{
    Lagu *lagu;
    PlaylistLaguNode *next;
};

struct Playlist
{
    char nama[100];
    PlaylistLaguNode *headLagu;
    Playlist *next;
};

struct Lirik
{
    char judul[MAX_JUDUL];
    char isi[MAX_LIRIK];
};

Lagu *headLagu = NULL;
Playlist *headPlaylist = NULL;

Lirik dbLirik[MAX_LIRIK_DB]; // ukuran 50
int jumlahLirik = 0;

char historyLagu[MAX_HISTORY][MAX_JUDUL]; // judul 100 & history 10
int jumlahHistory = 0;

// =================== FUNGSI BANTUAN =======================

void clearInput()
{
    cin.clear();            // menghapus eror dna melanjutkan kode *contoh misal user input huruf saat diminta angka lalu programa tidak peduli dan lanjut
    cin.ignore(1000, '\n'); // menghapus sisa input di variabel buffer hingga enter
}

void inputBaris(char *buffer, int size)
{                              // input baris pada variabel sementara "buffer" dengan ukuran yang dibataskan
    cin.getline(buffer, size); // input baris dengan batas size
}

void tambahLirik(const char *judul, const char *isi)
{
    for (int i = 0; i < jumlahLirik; i++)
    { // Cek apakah lirik untuk judul ini sudah ada, jika ya, update isinya
        if (strcmp(dbLirik[i].judul, judul) == 0)
        {
            strncpy(dbLirik[i].isi, isi, MAX_LIRIK - 1);
            dbLirik[i].isi[MAX_LIRIK - 1] = '\0';
            return;
        }
    }

    if (jumlahLirik < MAX_LIRIK_DB)
    { // Jika belum ada, tambah baru
        strncpy(dbLirik[jumlahLirik].judul, judul, MAX_JUDUL - 1);
        strncpy(dbLirik[jumlahLirik].isi, isi, MAX_LIRIK - 1);
        dbLirik[jumlahLirik].judul[MAX_JUDUL - 1] = '\0'; //'\0' agar string berakhir ga loop
        dbLirik[jumlahLirik].isi[MAX_LIRIK - 1] = '\0';
        jumlahLirik++;
    }
}

void simpanHistory(const char *judul)
{ // Menyimpan lagu ke history
    if (jumlahHistory < MAX_HISTORY)
    {                                                              // ngecek apakah masih ada slot kosong di history dalam hal ini ada 10 slot awal dan ini belum penuh
        strncpy(historyLagu[jumlahHistory], judul, MAX_JUDUL - 1); // maka dimasukin ke slot kosong itu dari ujung
        jumlahHistory++;                                           // indec bertambah
    }
    else
    {
        for (int i = 1; i < MAX_HISTORY; i++)                        // jika sudah penuh, geser semua history ke kiri
            strcpy(historyLagu[i - 1], historyLagu[i]);              // dan untuk slot terakhir diisi dengan lagu terbaru, lagu terlama dibuang dari baris atau di tandain denga index 0
        strncpy(historyLagu[MAX_HISTORY - 1], judul, MAX_JUDUL - 1); // membuang lagu terlama dan menambahkan lagu terbaru di akhir
    }
}

void tampilkanHistory()
{
    cout << "\n=== History lagu terakhir ===\n";
    if (jumlahHistory == 0)
    {
        cout << "(Belum ada)\n";
        return;
    }
    for (int i = 0; i < jumlahHistory; i++)
    {
        cout << i + 1 << ". " << historyLagu[i] << "\n";
    }
}

Lagu *buatNodeLagu(const char *judul, const char *artist, const char *genre)
{                             // membuat node lagu baru
    Lagu *newNode = new Lagu; // alokasi memori untuk node baru
    strncpy(newNode->judul, judul, MAX_JUDUL - 1);
    strncpy(newNode->artist, artist, MAX_ARTIST - 1);
    strncpy(newNode->genre, genre, MAX_GENRE - 1);
    newNode->favorit = false; // default bukan favorit
    newNode->next = NULL;     // inisialisasi next ke NULL
    return newNode;
}

// Agar saat load dari file + hardcode, lagunya tidak dobel
bool cekLaguSudahAda(const char *judulCari)
{
    Lagu *currentLagu = headLagu;
    while (currentLagu != NULL)
    {
        if (strcmp(currentLagu->judul, judulCari) == 0)
        {
            return true; // Lagu sudah ada
        }
        currentLagu = currentLagu->next;
    }
    return false; // Lagu belum ada
}
// ===================== SORT LAGU ==========================
// Tukar isi 2 struct Lagu
void swapLagu(Lagu *a, Lagu *b)
{
    Lagu temp = *a;
    *a = *b;
    *b = temp;
}

// Partition untuk quicksort, untuk membandingkan judul
int partitionLagu(Lagu arr[], int low, int high)
{

    // pivot = judul lagu elemen paling kanan
    char pivot[MAX_JUDUL];
    strcpy(pivot, arr[high].judul);
    int i = low - 1; // batas elemen yang <= pivot
    // ccan semua elemen sebelum pivot
    for (int j = low; j < high; j++)
    {
        // jika judul lebih kecil/sama pivot → pindah ke depan
        if (strcmp(arr[j].judul, pivot) <= 0)
        {
            i++;
            swapLagu(&arr[i], &arr[j]);
        }
    }
    // taruh pivot ke posisi finalnya
    swapLagu(&arr[i + 1], &arr[high]);
    return i + 1; // posisi pivot
}

// quicksort rekursif
void quickSortLagu(Lagu arr[], int low, int high)
{
    if (low < high)
    {
        // dapatkan pivot index
        int pi = partitionLagu(arr, low, high);
        // sort bagian kiri dan kanan pivot
        quickSortLagu(arr, low, pi - 1);
        quickSortLagu(arr, pi + 1, high);
    }
}

// convert linked list > array > sort > bangun ulang linked list
void sortLagu()
{
    // hitung jumlah node
    int count = 0;
    Lagu *p = headLagu;
    while (p != NULL)
    {
        count++;
        p = p->next;
    }
    if (count <= 1)
        return; // Tidak perlu sort
    // Copy isi linked list ke array
    Lagu *arr = (Lagu *)malloc(sizeof(Lagu) * count);
    int i = 0;
    p = headLagu;
    while (p != NULL)
    {
        arr[i++] = *p; // Copy seluruh struct
        p = p->next;
    }
    // Sort array dengan quicksort
    quickSortLagu(arr, 0, count - 1);
    // Bangun ulang linked list berdasarkan array yg sudah disort
    headLagu = NULL;
    for (int k = count - 1; k >= 0; k--)
    {
        Lagu *node = new Lagu; // buat node baru
        *node = arr[k];        // isi node = struct dari array
        node->next = headLagu; // sisipkan di depan
        headLagu = node;
    }
    free(arr); // bebas memori array
}
// ==================== FITUR SAVE & LOAD ====================

// Fungsi menyimpan data Linked List Lagu ke file txt
void simpanDataLagu()
{
    ofstream file("data_lagu.txt"); // Membuka file untuk menulis

    if (!file.is_open())
    {
        cout << "> Gagal menyimpan file!\n";
        return;
    }

    Lagu *currentLagu = headLagu;
    // Simpan data apa adanya
    while (currentLagu != NULL)
    {
        file << currentLagu->judul << endl;
        file << currentLagu->artist << endl;
        file << currentLagu->genre << endl;
        file << currentLagu->favorit << endl;
        currentLagu = currentLagu->next;
    }

    file.close();
    cout << "> Data lagu berhasil disimpan ke 'data_lagu.txt'.\n";
}

// Fungsi membaca file txt dan memasukkannya kembali ke Linked List
void muatDataLagu()
{
    ifstream file("data_lagu.txt"); // Membuka file untuk membaca

    if (!file.is_open())
    {
        return;
    }

    char judul[MAX_JUDUL], artist[MAX_ARTIST], genre[MAX_GENRE];
    bool favorit;

    // Loop membaca 4 baris sekaligus (Judul, Artist, Genre, Favorit)
    while (file.getline(judul, MAX_JUDUL))
    {
        file.getline(artist, MAX_ARTIST);
        file.getline(genre, MAX_GENRE);
        file >> favorit;
        file.ignore(); // Abaikan enter setelah angka bool

        // Cek dulu, kalau di list sudah ada (mungkin duplikat), skip atau tetap add?
        // Di sini kita add saja, nanti filter duplikatnya saat insert Hardcode.
        Lagu *node = buatNodeLagu(judul, artist, genre);
        node->favorit = favorit;

        // Insert di Head
        node->next = headLagu;
        headLagu = node;
    }

    file.close();
    cout << "> Data lagu berhasil dimuat dari file.\n";
}

// ==================== PENCARIAN LAGU ====================

void tampilkanSemuaLagu()
{
    sortLagu(); // lagu di sort
    if (!headLagu)
    {
        cout << "Belum ada lagu.\n";
        return;
    }
    cout << "\n~~~___--- DAFTAR LAGU ---___~~~\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    int i = 1;
    Lagu *currentLagu = headLagu;
    while (currentLagu != NULL)
    { // selama lagu ada maka...
        cout << i << ". " << currentLagu->judul << " - " << currentLagu->artist
             << " (" << currentLagu->genre << ")";
        if (currentLagu->favorit)
            cout << "";
        cout << "\n";
        i++;
        currentLagu = currentLagu->next;
    }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void cariJudul()
{
    tampilkanSemuaLagu();
    if (!headLagu)
    {
        cout << "> Belum ada lagu.\n";
        return;
    }
    char key[100];
    cout << "> Judul yang dicari: ";
    inputBaris(key, 100);

    Lagu *currentLagu = headLagu;
    bool ada = false;
    while (currentLagu != NULL)
    {
        if (strstr(currentLagu->judul, key) != NULL)
        {
            cout << "- " << currentLagu->judul << " - " << currentLagu->artist << " (" << currentLagu->genre << ")";
            if (currentLagu->favorit)
                cout << "";
            cout << "\n";
            ada = true;
        }
        currentLagu = currentLagu->next;
    }
    if (!ada)
        cout << "> Tidak ada lagu.\n";
}

void cariGenre()
{
    tampilkanSemuaLagu();
    if (!headLagu)
    {
        cout << "> Belum ada lagu.\n";
        return;
    }

    char key[100];
    cout << "> Genre yang dicari: ";
    inputBaris(key, 100);

    Lagu *currentLagu = headLagu;
    bool ada = false;
    while (currentLagu != NULL)
    {
        if (strstr(currentLagu->genre, key) != NULL)
        {
            cout << "- " << currentLagu->judul << " - " << currentLagu->artist << " (" << currentLagu->genre << ")";
            if (currentLagu->favorit)
                cout << "";
            cout << "\n";
            ada = true;
        }
        currentLagu = currentLagu->next;
    }
    if (!ada)
        cout << "> Tidak ada lagu.\n";
}

void cariArtist()
{
    tampilkanSemuaLagu();
    if (!headLagu)
    {
        cout << "> Belum ada lagu.\n";
        return;
    }

    char key[100];
    cout << "> Artist yang dicari: ";
    inputBaris(key, 100);

    Lagu *currentLagu = headLagu;
    bool ada = false;
    while (currentLagu != NULL)
    {
        if (strstr(currentLagu->artist, key) != NULL)
        {
            cout << "- " << currentLagu->judul << " - " << currentLagu->artist << " (" << currentLagu->genre << ")";
            if (currentLagu->favorit)
                cout << "";
            cout << "\n";
            ada = true;
        }
        currentLagu = currentLagu->next;
    }
    if (!ada)
        cout << "> Tidak ada lagu.\n";
}

// ==================== EDIT LAGU ====================

void menuInputLirik()
{
    if (!headLagu)
    { //  lagu tidak ada di variabel headLagu
        cout << "> Daftar lagu kosong. Tambah lagu dulu.\n";
        return;
    }

    tampilkanSemuaLagu();
    cout << "\n~~~___--- TAMBAH LIRIK ---___~~~\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    char judulInput[MAX_JUDUL]; // input judul dengan ukuran maxnya 100
    cout << "> Judul lagu: ";
    cin.getline(judulInput, MAX_JUDUL); // menyimpan inputan judul lagudari user termasuk ke variabel judulInput sesuai ukuran maxnya

    // Validasi apakah lagu ada di daftar wntah dari database atau inputan manual user
    if (!cekLaguSudahAda(judulInput))
    {
        cout << "> Gagal: Lagu dengan judul " << judulInput << " tidak ditemukan!\n";
        return;
    }

    cout << "> Masukkan lirik baris per baris.\n";
    cout << "> Ketik '0' (angka nol) pada baris baru lalu ENTER untuk selesai.\n";
    cout << "-----------------------------------------------------------------------\n";

    char buffer[500];
    char lirikGabungan[MAX_LIRIK] = ""; // Menyimpan lirik gabungan

    while (true)
    {
        cout << "-> ";            // Indikator baris baru untuk tambahkan lirik
        cin.getline(buffer, 500); // meyimpan inputan lirik per baris ke variabel buffer termasuk enter dan simbol lain sesuai ukuran

        if (strcmp(buffer, "0") == 0) // Jika user mengetik "0", berhenti
            break;

        if (strlen(lirikGabungan) + strlen(buffer) + 2 < sizeof(lirikGabungan))
        {                                  // Cek kapasitas buffer lirikGabungan agar tidak overflow saat menambahkan lirik per baris setelah ditambahkan 2 baris untuk "enter" dan "\0"
            strcat(lirikGabungan, buffer); // Gabungkan lirik baru dari variabel sementara "buffer" ke lirikGabungan
            strcat(lirikGabungan, "\n");   // Tambahkan enter di setiap akhir baris
        }
        else
        {
            cout << "> Lirik terlalu panjang, sebagian tidak akan tersimpan.\n";
            break;
        }
    }

    tambahLirik(judulInput, lirikGabungan); // ini buat tau masukin lirik ke mana sesuai judul lagunya
    cout << "-----------------------------------------------------------------------\n";
    cout << "Lirik berhasil disimpan untuk lagu: " << judulInput << "\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void toggleFavorite()
{
    if (!headLagu)
    {
        cout << "> Belum ada lagu.\n";
        return;
    }
    tampilkanSemuaLagu();
    cout << "> Pilih nomor lagu untuk di favoritkan: ";
    int pilihan;
    cin >> pilihan;
    clearInput();

    Lagu *currentLagu = headLagu;
    int idx = 1;
    while (currentLagu != NULL && idx < pilihan)
    {
        currentLagu = currentLagu->next;
        idx++;
    }
    if (!currentLagu)
    {
        cout << "> Nomor tidak valid!\n";
        return;
    }

    currentLagu->favorit = !currentLagu->favorit;
    if (currentLagu->favorit)
        cout << currentLagu->judul << " sekarang menjadi FAVORIT [♥]\n";
    else
        cout << currentLagu->judul << " tidak lagi menjadi favorit.\n";
}

void tambahLagu()
{
    char judul[MAX_JUDUL], artist[MAX_ARTIST], genre[MAX_GENRE];
    cout << "> Judul  : ";
    inputBaris(judul, MAX_JUDUL);
    cout << "> Artist : ";
    inputBaris(artist, MAX_ARTIST);
    cout << "> Genre  : ";
    inputBaris(genre, MAX_GENRE);

    // Cek duplikasi saat input manual
    if (cekLaguSudahAda(judul))
    {
        cout << "> Lagu sudah ada di daftar!\n";
        return;
    }

    Lagu *node = buatNodeLagu(judul, artist, genre);
    node->next = headLagu;
    headLagu = node;
    cout << "> Lagu berhasil ditambahkan!\n";
}

void hapusLagu()
{
    if (!headLagu)
    {
        cout << "Belum ada lagu.\n";
        return;
    } // kalo Headlagu = null
    tampilkanSemuaLagu();
    cout << "Nomor lagu yang dihapus: ";
    int pilihan;
    cin >> pilihan;
    clearInput();

    Lagu *currentLagu = headLagu; // nunjuk ke lagu pertama
    Lagu *prev = NULL;
    int i = 1;
    while (currentLagu != NULL && i < pilihan)
    {
        prev = currentLagu;
        currentLagu = currentLagu->next;
        i++;
    }
    // loop sampai ketemu lagu yang mau di hapus
    if (!currentLagu)
    {
        cout << "Nomor tidak valid.\n";
        return;
    }
    if (prev == NULL)
        headLagu = currentLagu->next; // headlagunya jadi ke next
    else
        prev->next = currentLagu->next; // misal hapus B jadi AC

    cout << currentLagu->judul << " dihapus.\n";
    delete currentLagu; // hapus lagu
}

void menuEditLagu()
{
    int pilihan;
    do
    {
        cout << "\n~~~___---* EDIT LAGU! *---___~~~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "1. Tambah lagu\n";
        cout << "2. Hapus lagu\n";
        cout << "3. Tampilkan semua lagu\n";
        cout << "4. Favorite ON/OFF\n";
        cout << "5. Tambahkan lirik\n";
        cout << "0. Kembali\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "> Pilih: ";
        cin >> pilihan;
        clearInput();

        switch (pilihan)
        {
        case 1:
            tambahLagu();
            break;
        case 2:
            hapusLagu();
            break;
        case 3:
            tampilkanSemuaLagu();
            break;
        case 4:
            toggleFavorite();
            break;
        case 5:
            menuInputLirik();
            break;
        case 0:
            break;
        default:
            cout << "> Pilihan tidak ada!\n";
        }
    } while (pilihan != 0);
}

// ==================== MANAJEMEN PLAYLIST ====================

void tampilkanDaftarPlaylist()
{
    if (!headPlaylist)
    {
        cout << "(Belum ada playlist yang dibuat)\n"; // headPlaylist = null
        return;
    }
    cout << "\n--- LIST PLAYLIST ---\n";
    Playlist *ptrPlaylist = headPlaylist; // nunjuk ke node playlist
    int i = 1;
    while (ptrPlaylist != NULL)
    {
        cout << i << ". " << ptrPlaylist->nama << endl;
        ptrPlaylist = ptrPlaylist->next;
        i++;
    }
}

void buatPlaylistBaru()
{
    char namaPl[100];
    cout << "Masukkan nama playlist baru: ";
    inputBaris(namaPl, 100);

    Playlist *baru = new Playlist;
    strncpy(baru->nama, namaPl, 99);
    baru->headLagu = NULL;
    baru->next = headPlaylist;
    headPlaylist = baru;

    cout << "Playlist '" << namaPl << "' berhasil dibuat!\n";
}

void gantiNamaPlaylist()
{
    if (!headPlaylist)
    {
        cout << "Belum ada playlist.\n";
        return;
    }
    tampilkanDaftarPlaylist();
    cout << "Pilih nomor playlist: ";
    int pilihan;
    cin >> pilihan;
    clearInput();
    Playlist *ptrPlaylist = headPlaylist;
    int idx = 1;
    while (ptrPlaylist != NULL && idx < pilihan)
    {
        ptrPlaylist = ptrPlaylist->next;
        idx++;
    }
    if (ptrPlaylist == NULL)
        cout << "Nomor playlist tidak valid.\n";
    else
    {
        cout << "Masukkan nama baru: ";
        inputBaris(ptrPlaylist->nama, 100);
        cout << "Nama playlist berhasil diubah!\n";
    }
}

void hapusPlaylist()
{
    if (!headPlaylist)
    {
        cout << "Belum ada playlist.\n";
        return;
    }
    tampilkanDaftarPlaylist();
    cout << "Pilih nomor playlist yang akan DIHAPUS: ";
    int pilihan;
    cin >> pilihan;
    clearInput();

    Playlist *curr = headPlaylist;
    Playlist *prev = NULL;
    int idx = 1;

    while (curr != NULL && idx < pilihan)
    {
        prev = curr;
        curr = curr->next;
        idx++;
    }

    if (curr == NULL)
    {
        cout << "Nomor tidak valid.\n";
        return;
    }

    PlaylistLaguNode *nodeLagu = curr->headLagu;
    while (nodeLagu != NULL)
    {
        PlaylistLaguNode *temp = nodeLagu;
        nodeLagu = nodeLagu->next;
        delete temp;
    }

    if (prev == NULL)
        headPlaylist = curr->next;
    else
        prev->next = curr->next;

    cout << "Playlist '" << curr->nama << "' berhasil dihapus permanen.\n";
    delete curr;
}

void menuEditPlaylist()
{
    int pilihan;
    do
    {
        cout << "\n~~~___--- Edit Playlist ---___~~~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "1. Buat Playlist Baru\n";
        cout << "2. Ganti Nama Playlist\n";
        cout << "3. Hapus Playlist\n";
        cout << "4. Tampilkan Daftar Playlist\n";
        cout << "0. Kembali ke Menu Utama\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "Pilih: ";
        cin >> pilihan;
        clearInput(); // bersihin input dari variabel buffer

        switch (pilihan)
        {
        case 1:
            buatPlaylistBaru();
            break;
        case 2:
            gantiNamaPlaylist();
            break;
        case 3:
            hapusPlaylist();
            break;
        case 4:
            tampilkanDaftarPlaylist();
            break;
        case 0:
            break;
        default:
            cout << "Pilihan tidak ada!\n";
        }
    } while (pilihan != 0);
}

// ==================== DENGERIN LAGU ====================

int hitungLagu()
{ // mengurutkan semua lagu dna menghitung jumlahnya
    int c = 0;
    Lagu *ptr = headLagu; // dari alamat variabel lagu di pindahkan sementara ke ptr dengan nama headLagu
    while (ptr != NULL)
    { // selama ptr tidak kosong
        c++;
        ptr = ptr->next;
    }
    return c;
}

Lagu *getLaguByIndex(int idx)
{                         // mengambil lagu berdasarkan index
    Lagu *ptr = headLagu; // mulai dari headLagu
    int i = 0;            // index awal 0
    while (ptr != NULL)
    {
        if (i == idx)
            return ptr;  // jika index sama dengan idx maka kembalikan ptr
        ptr = ptr->next; // lanjut ke lagu berikutnya
        i++;             // index bertambah
    }
    return NULL; // jika tidak ditemukan kembalikan NULL
}

void putarLagu(Lagu *l)
{
    cout << "\nMemutar: " << l->judul << " - " << l->artist; // input lagu berdasarkan judul dan artist
    if (l->favorit)
        cout << " [♥]"; // maupun favorit
    cout << "\n";

    bool adaLirik = false; // mengecek apakah lirik ada atau tidak
    for (int i = 0; i < jumlahLirik; i++)
    { // jika ada lirik amak akan loop sebanyak jumlah lirik
        if (strcmp(dbLirik[i].judul, l->judul) == 0)
        {
            cout << "\n~~~___---=== !LIRIK! ===---___~~~\n";
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << dbLirik[i].isi << "\n"; // menampilkan isi lirik berdasarkan judul lagu yang diinputkan oleh user
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            adaLirik = true; // shg ada liriknya
            break;
        }
    }
    if (!adaLirik)
        cout << "(Lirik belum tersedia)\n"; // jika ga ada maka outputnya ...

    simpanHistory(l->judul); // menyimpan lagu ke history
}

void putarLaguPilihan()
{
    if (!headLagu)
    {
        cout << "Belum ada lagu.\n";
        return;
    }
    tampilkanSemuaLagu(); // menampilakan semua lagu pada menu utaam yaitu dengerin lagu

    cout << "Pilih nomor lagu yang ingin diputar: "; // ini yang pilih lagu manual
    int pilihan;
    cin >> pilihan;
    clearInput(); // bersihin input dari variabel buffer

    Lagu *ptr = headLagu; // mulai dari headLagu // ini yang random
    int idx = 1;          // indx dimulai dari 1
    while (ptr != NULL && idx < pilihan)
    { // bakal ngegeser terus selama ptr tidak kosong dan index kurang dari 1
        ptr = ptr->next;
        idx++; // lanjut ke lagu berikutnya dan index bertambah
    }

    if (ptr != NULL)
        putarLagu(ptr); // jika ptr tidak kosong maka putar lagu berdasarkan ptr
    else
        cout << "Nomor lagu tidak valid!\n";
}

void putarLaguFavorit()
{
    if (!headLagu)
    {
        cout << "Lagu kosong.\n";
        return;
    }
    cout << "\n~~~___--- !Lagu Favorit! ---___~~~\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    Lagu *ptr = headLagu;
    int count = 0; // hitung lagu favorit
    int index = 1; // nomor urut favorit

    while (ptr != NULL)
    { // Tampilkan hanya yang favorit
        if (ptr->favorit)
        {
            cout << index << ". " << ptr->judul << " - " << ptr->artist << "\n";
            count++; // jumlah lagu favorit bertambah
            index++; // nomor urut favorit bertambah
        }
        ptr = ptr->next; // lanjut ke lagu berikutnya
    }

    if (count == 0)
    {
        cout << "(Belum ada lagu favorit. Silakan 'like' lagu dulu di menu edit)\n";
        return;
    }

    cout << "Pilih nomor urut dari daftar favorit di atas: ";
    int pilihan;
    cin >> pilihan;
    clearInput();

    ptr = headLagu;
    int currentIdx = 1; // nomor urut favorit saat ini
    while (ptr != NULL)
    {
        if (ptr->favorit)
        {
            if (currentIdx == pilihan)
            {
                putarLagu(ptr);
                return;
            }
            currentIdx++;
        }
        ptr = ptr->next;
    }
    cout << "Nomor tidak valid.\n";
}

void randomLagu()
{                         // memutar lagu random
    int n = hitungLagu(); // menghitung jumlah lagu
    if (n == 0)
    {
        cout << "Tidak ada lagu.\n";
        return;
    }
    int pick = rand() % n;           // memilih angka random dari 0 sampai n-1
    putarLagu(getLaguByIndex(pick)); // memutar lagu berdasarkan index yang di random
}

void menuDengerin()
{
    int pilihan;
    do
    {
        cout << "\n~~~___--- Mendengar Lagu ---___~~~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "1. Putar random\n";
        cout << "2. Tampilkan semua lagu\n";
        cout << "3. Pilih lagu untuk diputar\n";
        cout << "4. Putar Playlist Favorit [♥]\n";
        cout << "0. Kembali\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "Pilih: ";
        cin >> pilihan;
        clearInput();

        switch (pilihan)
        {
        case 1:
            randomLagu();
            break;
        case 2:
            tampilkanSemuaLagu();
            break;
        case 3:
            putarLaguPilihan();
            break;
        case 4:
            putarLaguFavorit();
            break;
        case 0:
            break;
        default:
            cout << "Pilihan tidak ada!\n";
        }
    } while (pilihan != 0);
}

// ==================== MAIN PROGRAM ====================

int main()
{
    const char *GREEN = "\033[1;32m";
    const char *BLUE = "\033[1;34m";
    const char *YELLOW = "\033[1;33m";
    const char *RESET = "\033[0m";

    cout << "/// " << BLUE << "╔════════════════════════════════════════════════════════════════════╗" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║                                                                    ║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║                           " << GREEN << "WELCOME TO" << RESET << "                               " << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║                                                                    ║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << BLUE << " __  __   _    _     _____    _____     _____    ______   __     __   " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << BLUE << "|  \\/  | | |  | |   / ____| |_   _|   / ____| |   ____| \\ \\   / /   " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << BLUE << "| \\  / | | |  | | | (___      | |    | |       | |__      \\ \\_/ /     " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << YELLOW << "| |\\/| | | |  | |  \\___ \\     | |    | |       |  __|      \\   /      " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << YELLOW << "| |  | | | |__| |   ____) |  _| |_   | |____   | |          | |       " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║   " << YELLOW << "|_|  |_|  \\____/   |_____/   |_____|  \\_____| |_|          |_|       " << RESET << BLUE << "║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "║                                                                    ║" << RESET << " ///" << endl;
    cout << "/// " << BLUE << "╚════════════════════════════════════════════════════════════════════╝" << RESET << " ///" << endl;

    srand((unsigned)time(NULL));
    muatDataLagu();

    // Database lagu
    if (!cekLaguSudahAda("Terbuang Dalam Waktu"))
    { // di cek di fitur dengerin lagu apakah lagu ada untuk dipuatar
        Lagu *l1 = buatNodeLagu("Terbuang Dalam Waktu", "Unknown Artist", "Pop");
        l1->next = headLagu;
        headLagu = l1;
    }
    if (!cekLaguSudahAda("Mangu"))
    {
        Lagu *l2 = buatNodeLagu("Mangu", "Unknown Artist", "Indie");
        l2->next = headLagu;
        headLagu = l2;
    }
    if (!cekLaguSudahAda("A Million Dreams"))
    {
        Lagu *l3 = buatNodeLagu("A Million Dreams", "Unknown Artist", "Soundtrack");
        l3->next = headLagu;
        headLagu = l3;
    }

    // database lirik lagu
    tambahLirik("Terbuang Dalam Waktu", "\nTeringat seru suaramu menepis keraguan...\nAngan tenggelam dalam kabut dan amarah...");
    tambahLirik("Mangu", "\nSuatu malam Adam bercerita...\nHawa-nya tak lagi di jalur yang sama...");
    tambahLirik("A Million Dreams", "\nI close my eyes and I can see...\nThe world that's waiting up for me...");

    int menu;
    do
    {
        cout << "\n~~~___---* !Menu Utama! *---___~~~\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "1. Cari Lagu\n";
        cout << "2. Edit Lagu\n";
        cout << "3. Edit Playlist\n";
        cout << "4. Dengerin Lagu\n";
        cout << "0. Simpan & Keluar\n";
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "Pilih: ";
        cin >> menu;
        clearInput();

        switch (menu)
        {
        case 1:
            int sub;
            cout << "\n~~~___---** Cari Lagu! **---___~~~\n";
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "\n1. Judul\n2. Genre\n3. Artist\n4. History\n";
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "Pilih: ";
            cin >> sub;
            clearInput();
            if (sub == 1)
                cariJudul();
            else if (sub == 2)
                cariGenre();
            else if (sub == 3)
                cariArtist();
            else if (sub == 4)
                tampilkanHistory();
            break;

        case 2:
            menuEditLagu();
            break;
        case 3:
            menuEditPlaylist();
            break;
        case 4:
            menuDengerin();
            break;
        case 0: // keluar & simpan data ke file
            simpanDataLagu();
            cout << "Terimakasih telah menggunakan Musik Fy\n";
            break;
        default:
            cout << "Menu tidak tersedia.\n";
        }

    } while (menu != 0);

    return 0;
}