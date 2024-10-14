// Definisikan pin GPIO untuk a-g
const int pins[] = {16, 4, 0, 2, 15, 17, 5}; // a, b, c, d, e, f, g
const int tombol[] = {33, 25, 26}; // 3 tombol untuk 3 peserta
// Definisikan pin untuk common
const int common_pins[] = {18, 21, 22}; // common
const int juri[] = {34, 35, 32};
int nilai_peserta[3] = {0, 0, 0};
int pesertaAktif = 0;
bool tombolDitekan = false; // Variabel untuk memeriksa apakah sudah ada tombol yang ditekan
const int angka_7seg[10][7] = {
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},  // 0
    {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},      // 1
    {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},   // 2
    {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},   // 3
    {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},    // 4
    {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},   // 5
    {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},  // 6
    {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},     // 7
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // 8
    {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}   // 9
};


void setup() {
    Serial.begin(115200);
    
    // Inisialisasi semua pin a-g sebagai OUTPUT
    for (int i = 0; i < 7; i++) {
        pinMode(pins[i], OUTPUT);
    }
    
    // Inisialisasi semua pin common sebagai OUTPUT
    for (int i = 0; i < 3; i++) {
        pinMode(common_pins[i], OUTPUT);
    }

    // Inisialisasi semua tombol sebagai INPUT_PULLUP
    for (int i = 0; i < 3; i++) {
        pinMode(tombol[i], INPUT_PULLUP);
    }

    // Inisialisasi tombol juri sebagai INPUT_PULLUP
    for (int i = 0; i < 3; i++) {
        pinMode(juri[i], INPUT_PULLUP);
    }

    // Contoh pengaturan status pin
    digitalWrite(pins[0], HIGH); // Aktifkan pin a
    digitalWrite(pins[1], HIGH); // Aktifkan pin b
    digitalWrite(pins[2], HIGH); // Aktifkan pin c
    digitalWrite(pins[3], HIGH); // Aktifkan pin d
    digitalWrite(pins[4], HIGH); // Aktifkan pin e
    digitalWrite(pins[5], HIGH); // Aktifkan pin f
    digitalWrite(pins[6], LOW);  // Nonaktifkan pin g
    
    // Mengatur status pin common
    for (int i = 0; i < 3; i++) {
        digitalWrite(common_pins[i], LOW); // Menonaktifkan semua pin common
    }

    // Tampilkan angka awal (nilai peserta 0)
    tampilkanAngka(0);
    // Serial.println(pesertaAktif + "YAAA");
}

void loop() {
    if (tombolDitekan == false) {  // Jika belum ada tombol yang ditekan
        for (int i = 0; i < 3; i++) {
            if (digitalRead(tombol[i]) == LOW) {  // Jika tombol ditekan
                tombolDitekan = true;  // Mengunci agar hanya tombol pertama yang dihitung
                pesertaAktif = i;  // Update pesertaAktif dengan indeks peserta yang menekan tombol
                // Matikan semua common pin kecuali yang sesuai dengan tombol yang ditekan
                for (int j = 0; j < 3; j++) {
                    if (i == j) {
                        digitalWrite(common_pins[j], LOW);  // Tetap LOW (aktif) untuk tombol yang ditekan pertama
                    } else {
                        digitalWrite(common_pins[j], HIGH);  // HIGH (nonaktif) untuk common pin lainnya
                    }
                }
                break;  // Keluar dari loop setelah tombol pertama ditekan
            }
        }
    }


    // Cek tombol reset (juri pin 34)
    if (digitalRead(juri[0]) == LOW) {
        resetNilaiPeserta();
        
    }

    // Cek tombol naik (juri pin 35) - hanya bekerja jika ada peserta yang sudah menekan tombol
    if (digitalRead(juri[1]) == LOW && tombolDitekan) {
        tambahNilai();
        tampilkanSemuaPeserta();  // Tampilkan kembali nilai semua peserta

    }

    // Cek tombol turun (juri pin 32) - hanya bekerja jika ada peserta yang sudah menekan tombol
    if (digitalRead(juri[2]) == LOW && tombolDitekan) {
        kurangNilai();
        switchPeserta();
        tampilkanSemuaPeserta();  // Tampilkan kembali nilai semua peserta setelah pindah peserta
    }
}
// Fungsi untuk menampilkan angka di 7-segment
void tampilkanAngka(int angka) {
    for (int i = 0; i < 7; i++) {
        digitalWrite(pins[i], angka_7seg[angka][i]);
    }

}

// Fungsi untuk menampilkan nilai semua peserta
void tampilkanSemuaPeserta() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(common_pins[i], LOW);  // Aktifkan semua common pin (display peserta)
        tampilkanAngka(nilai_peserta[i]);   // Tampilkan nilai peserta
    }
}

// Fungsi untuk reset nilai semua peserta ke 0
void resetNilaiPeserta() {
    for (int i = 0; i < 3; i++) {
        nilai_peserta[i] = 0;  // Set semua nilai peserta ke 0
    }
    tampilkanAngka(0);  // Tampilkan 0 di 7-segment
    tombolDitekan = false;  // Set tombolDitekan kembali ke false agar juri tidak bisa menaikkan/mengurangi nilai
                        Serial.println(tombolDitekan);

}

// Fungsi untuk menambah nilai pada peserta aktif
void tambahNilai() {
  // Serial.println(nilai_peserta);
    if (nilai_peserta[pesertaAktif] < 9) {
        nilai_peserta[pesertaAktif]++;  // Tambah nilai peserta aktif
    }
    tampilkanAngka(nilai_peserta[pesertaAktif]);  // Tampilkan nilai yang baru
}
// Fungsi untuk mengurangi nilai pada peserta aktif
void kurangNilai() {
    if (nilai_peserta[pesertaAktif] > 0) {
        nilai_peserta[pesertaAktif]--;  // Kurangi nilai peserta aktif
    }
    tampilkanAngka(nilai_peserta[pesertaAktif]);  // Tampilkan nilai yang baru
}

// Fungsi untuk beralih ke peserta berikutnya
void switchPeserta() {
    digitalWrite(common_pins[pesertaAktif], HIGH);  // Matikan display peserta yang aktif
    pesertaAktif = (pesertaAktif + 1) % 3;  // Beralih ke peserta berikutnya
    digitalWrite(common_pins[pesertaAktif], LOW);   // Nyalakan display peserta yang baru
    tampilkanAngka(nilai_peserta[pesertaAktif]);  // Tampilkan nilai peserta yang baru
}