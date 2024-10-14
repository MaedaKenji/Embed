#include <Arduino.h>

// Definisikan pin GPIO untuk a-g
const int pins[] = {16, 4, 0, 2, 15, 17, 5}; // a, b, c, d, e, f, g
const int tombol[] = {33, 25, 26};           // 3 tombol untuk 3 peserta
// Definisikan pin untuk common
const int common_pins[] = {18, 21, 22}; // common
const int juri[] = {34, 35, 32};
int nilai_peserta[3] = {5, 5, 5};
int pesertaAktif = -1;
int jumlahPesertaAktif = 3;
bool tombolSudahDitekan = false; // Variabel untuk memeriksa apakah sudah ada tombol yang ditekan
// bool prevJuriState[3] = {HIGH, HIGH, HIGH};
bool prevJuriState[3] = {};
bool currentDecreaseState = HIGH;
bool prevIncreaseState = HIGH;
bool semua = true;
const int MAX_PESERTA = 3;
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
struct PesertaStates
{
    bool states[MAX_PESERTA];

    PesertaStates()
    {
        for (int i = 0; i < MAX_PESERTA; i++)
        {
            states[i] = true; // Inisialisasi semua peserta sebagai aktif
        }
    }

    void setActive(int index)
    {
        if (index >= 0 && index < MAX_PESERTA)
        {
            states[index] = true;
        }
    }

    void setInactive(int index)
    {
        if (index >= 0 && index < MAX_PESERTA)
        {
            states[index] = false;
        }
    }

    bool isActive(int index) const
    {
        return (index >= 0 && index < MAX_PESERTA) ? states[index] : false;
    }

    int getActiveCount() const
    {
        int count = 0;
        for (int i = 0; i < MAX_PESERTA; i++)
        {
            if (states[i])
                count++;
        }
        return count;
    }
};

PesertaStates pesertaStates;

void tampilkanAngka(int angka, int displayIndex)
{
    // Nonaktifkan semua common pin
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(common_pins[i], HIGH);
    }

    // Set nilai pins sesuai dengan angka yang akan ditampilkan
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(pins[i], angka_7seg[angka][i]);
    }

    // Aktifkan common pin yang sesuai
    digitalWrite(common_pins[displayIndex], LOW);
}

void tampilkanSemuaPeserta()
{
    for (int i = 0; i < 3; i++)
    {
        tampilkanAngka(nilai_peserta[i], i);
        delay(5); // Delay kecil untuk multiplexing
    }
}

int resetNilaiPeserta()
{
    for (int i = 0; i < 3; i++)
    {
        nilai_peserta[i] = 0;
    }
    // tombolDitekan = false;
    pesertaAktif = 0;
    return 0;
}

int tambahNilai()
{
    if (nilai_peserta[pesertaAktif] < 9)
    {
        nilai_peserta[pesertaAktif]++;
    }
    return nilai_peserta[pesertaAktif];
}

int kurangNilai()
{
    if (nilai_peserta[pesertaAktif] > 0)
    {
        nilai_peserta[pesertaAktif]--;
    }
    return nilai_peserta[pesertaAktif];
}

int switchPeserta()
{
    pesertaAktif = (pesertaAktif + 1) % 3;
    return pesertaAktif;
}

void setup()
{
    Serial.begin(115200);

    // Inisialisasi semua pin a-g sebagai OUTPUT
    for (int i = 0; i < 7; i++)
    {
        pinMode(pins[i], OUTPUT);
    }

    // Inisialisasi semua pin common sebagai OUTPUT
    for (int i = 0; i < 3; i++)
    {
        pinMode(common_pins[i], OUTPUT);
    }

    // Inisialisasi semua tombol sebagai INPUT_PULLUP
    for (int i = 0; i < 3; i++)
    {
        pinMode(tombol[i], INPUT_PULLUP);
    }

    // Inisialisasi tombol juri sebagai INPUT_PULLUP
    for (int i = 0; i < 3; i++)
    {
        pinMode(juri[i], INPUT_PULLUP);
        pesertaStates.setActive(i);
    }

    // Contoh pengaturan status pin
    // digitalWrite(pins[0], HIGH); // Aktifkan pin a
    // digitalWrite(pins[1], HIGH); // Aktifkan pin b
    // digitalWrite(pins[2], HIGH); // Aktifkan pin c
    // digitalWrite(pins[3], HIGH); // Aktifkan pin d
    // digitalWrite(pins[4], HIGH); // Aktifkan pin e
    // digitalWrite(pins[5], HIGH); // Aktifkan pin f
    // digitalWrite(pins[6], LOW);  // Nonaktifkan pin g

    // Mengatur status pin common
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(common_pins[i], HIGH); // Menonaktifkan semua pin common
    }

    // Tampilkan angka awal (nilai peserta 0)
    tampilkanSemuaPeserta();
    // Serial.println(pesertaAktif + "YAAA");
}

void loop()
{

    // Tampilkan semua peserta
    if (semua)
    {
        tampilkanSemuaPeserta();
    }

    if (!tombolSudahDitekan)
    {
        // Serial.println("Tombol belum ditekan");
        for (int i = 0; i < 3; i++)
        {
            if (digitalRead(tombol[i]) == LOW)
            {
                // Tombol peserta i ditekan
                tombolSudahDitekan = true;
                pesertaAktif = i;
                semua = false;

                for (int j = 0; j < 3; j++)
                {
                    digitalWrite(common_pins[j], HIGH);
                }
                if (pesertaAktif == 0)
                {
                    tampilkanAngka(nilai_peserta[0], 0);
                }
                else if (pesertaAktif == 1)
                {
                    tampilkanAngka(nilai_peserta[1], 1);
                }
                else if (pesertaAktif == 2)
                {
                    tampilkanAngka(nilai_peserta[2], 2);
                }
            }
        }
    }
    else
    {
        // Serial.println("Tombol sudah ditekan");
    }

    // Logika untuk tombol juri dan tampilan
    if (tombolSudahDitekan)
    {
        // Cek tombol reset (juri pin 34)
        if (digitalRead(juri[0]) == LOW && prevJuriState[0] == HIGH)
        {
            resetNilaiPeserta();
            tombolSudahDitekan = false; // Reset state tombol
            Serial.println("Reset");
            semua = true;
            // prevJuriState[0] = HIGH;
        }
        prevJuriState[0] = digitalRead(juri[0]);

        // Cek tombol naik (juri pin 35)
        if (digitalRead(juri[1]) == LOW && prevJuriState[1] == HIGH)
        {
            tambahNilai();
            // tampilkanAngka(nilai_peserta[pesertaAktif], pesertaAktif);
            Serial.println("Naik");
            semua = true;
            tombolSudahDitekan = false;
        }
        prevJuriState[1] = digitalRead(juri[1]);

        // Cek tombol turun (juri pin 32)
        if (digitalRead(juri[2]) == LOW && prevJuriState[2] == HIGH)
        {
            kurangNilai();
            // tampilkanAngka(nilai_peserta[pesertaAktif], pesertaAktif);
            // tombolSudahDitekan = false;
            jumlahPesertaAktif--;
            pesertaStates.setInactive(pesertaAktif);
            // Serial.println("Jumlah peserta aktif: " + String(jumlahPesertaAktif));
            // if (jumlahPesertaAktif <= 0) {
            //     semua = true;
            tombolSudahDitekan = false;
            bool keluarWhile = false;

            Serial.println("Turun 1");
            Serial.println("Peserta ke " + String(pesertaAktif) + "Nilainya berkurang menjadi " + String(nilai_peserta[pesertaAktif]));
            Serial.println("Jumlah peserta aktif: " + String(jumlahPesertaAktif));
            // }
            // else {
            while (jumlahPesertaAktif > 0)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (pesertaStates.isActive(i) && tombolSudahDitekan == false)
                    {
                        tampilkanAngka(nilai_peserta[i], i);
                        Serial.println("Peserta ke " + String(i) + "Keaktifannya " + String(pesertaStates.isActive(i)));
                    }
                    // else
                    // {
                    //     Serial.println("Tidak ada peserta aktif ");
                    //     keluarWhile = true;
                    //     break;
                    // }
                }

                // if (keluarWhile)
                // {
                //     break;
                // }

                if (!tombolSudahDitekan)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (digitalRead(tombol[i]) == LOW && pesertaStates.isActive(i))
                        {
                            // Tombol peserta i ditekan
                            tombolSudahDitekan = true;
                            pesertaAktif = i;
                            semua = false;

                            for (int j = 0; j < 3; j++)
                            {
                                digitalWrite(common_pins[j], HIGH);
                            }
                            if (pesertaStates.isActive(pesertaAktif))
                            {
                                tampilkanAngka(nilai_peserta[pesertaAktif], pesertaAktif);
                            }
                        }
                    }
                }
                else
                {
                    // Cek tombol naik (juri pin 35)
                    if (digitalRead(juri[1]) == LOW && prevJuriState[1] == HIGH)
                    {
                        tambahNilai();
                        // tampilkanAngka(nilai_peserta[pesertaAktif], pesertaAktif);
                        Serial.println("Naik");
                        semua = true;
                        tombolSudahDitekan = false;
                        break;
                    }
                    prevJuriState[1] = digitalRead(juri[1]);

                    if (digitalRead(juri[2]) == LOW && prevJuriState[2] == HIGH)
                    {
                        if (pesertaStates.isActive(pesertaAktif))
                        {
                            kurangNilai();
                            // tampilkanAngka(nilai_peserta[pesertaAktif], pesertaAktif);

                            // tombolSudahDitekan = false;
                            jumlahPesertaAktif--;
                            tombolSudahDitekan = false;
                            pesertaStates.setInactive(pesertaAktif);
                            Serial.println("Turun 2");
                            Serial.println("Peserta ke " + String(pesertaAktif) + "Nilainya berkurang menjadi " + String(nilai_peserta[pesertaAktif]));
                            Serial.println("Jumlah peserta aktif: " + String(jumlahPesertaAktif));
                            // Serial.println("Jumlah peserta aktif: " + String(jumlahPesertaAktif));
                        }
                    }
                    prevJuriState[2] = digitalRead(juri[2]);
                }
            }
            // }
            for (int i = 0; i < 3; i++)
            {
                pesertaStates.setActive(i);
            }
            delay(5);
            tombolSudahDitekan = false;
            semua = true;
            jumlahPesertaAktif = pesertaStates.getActiveCount();
        }
        // currentDecreaseState = digitalRead(juri[2]);
        // Serial.println(currentDecreaseState);
        prevJuriState[2] = digitalRead(juri[2]);
        // if (jumlahPesertaAktif == 3) {
        //     currentDecreaseState = digitalRead(juri[2]);
        // }
        // else {
        //     currentDecreaseState = HIGH;
        // }
    }

    // Delay kecil untuk debouncing
    delay(50);
}