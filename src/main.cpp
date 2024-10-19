#include <Arduino.h>

// Definisikan pin GPIO untuk a-g
const int pins[] = {16, 4, 0, 2, 15, 17, 5}; // a, b, c, d, e, f, g
const int tombol[] = {33, 25, 26};           // 3 tombol untuk 3 peserta
// Definisikan pin untuk common
const int common_pins[] = {18, 21, 22}; // common
const int juri[] = {27, 14, 32};
int nilai_peserta[3] = {5, 5, 5};
int pesertaAktif = -1;
int jumlahPesertaAktif = 3;
int kondisi = -1;                // 0: Peserta pertama menjawab salah
bool tombolSudahDitekan = false; // Variabel untuk memeriksa apakah sudah ada tombol yang ditekan
int timer = 0;
bool prevJuriState[3] = {};
bool currentDecreaseState = HIGH;
bool prevIncreaseState = HIGH;
bool semua = true;
bool standby = false;
const int MAX_PESERTA = 3;
unsigned long startTime = millis(); // Ambil waktu mulai
unsigned long limaBelasS = 15000;   // 15 detik dalam milidetik
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
const int huruf_7seg[3][7] = {
    {HIGH, HIGH, HIGH, HIGH, HIGH, LOW, HIGH}, // a
    {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},  // b
    {HIGH, LOW, LOW, HIGH, HIGH, HIGH, LOW}    // c
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

int tampilkanHuruf(int huruf, int displayIndex)
{
    if (huruf < 0 || huruf > 2)
    {
        Serial.println("Huruf tidak valid! Masukkan 0 untuk 'a', 1 untuk 'b', atau 2 untuk 'c'.");
        return -1;
    }

    for (int i = 0; i < 3; i++)
    {
        digitalWrite(common_pins[i], HIGH);
    }
    // Loop untuk mengatur segmen
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(pins[i], huruf_7seg[huruf][i]);
    }
    // Aktifkan common pin yang sesuai
    digitalWrite(common_pins[displayIndex], LOW);
    return 0;
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
        pesertaStates.setActive(i);
    }

    // Inisialisasi tombol juri sebagai INPUT_PULLUP
    for (int i = 0; i < 3; i++)
    {
        pinMode(juri[i], INPUT_PULLUP);
    }

    // Mengatur status pin common
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(common_pins[i], HIGH); // Menonaktifkan semua pin common
    }

    Serial.println("Program Started");
}

void loop()
{
    // if (tombolSudahDitekan) Serial.println("Tombol sudah ditekan: " + String(tombolSudahDitekan));
    // else Serial.println("Tombol belum ditekan: " + String(tombolSudahDitekan));
    if (false)
    {

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
                delay(5000);
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
    }

    // Tampilkan semua peserta
    if (!standby)
    {
        tampilkanSemuaPeserta();
        startTime = millis();
        Serial.println("Masuk ke !standby");
    }
    else if (standby && !tombolSudahDitekan)
    {
        Serial.println("Masuk ke standby");
        timer = millis() - startTime;
        if (timer < 15000 && !tombolSudahDitekan) // Selama waktu yang berlalu kurang dari 15 detik
        {
            Serial.println("Looping 1 selama " + String(timer) + " detik dan tombol ditekan adalah: " + String(tombolSudahDitekan));
            // Tampilkan huruf untuk peserta
            for (int i = 0; i < 3; i++)
            {
                tampilkanHuruf(i, i);

                // Cek jika tombol peserta ditekan
                for (int j = 0; j < 3; j++)
                {
                    if (digitalRead(tombol[j]) == LOW)
                    {
                        tombolSudahDitekan = true;
                        pesertaAktif = j;
                        Serial.println("Tombol ditekan pada peserta: " + String(pesertaAktif));
                        // standby = false;
                        break;
                    }
                }

                // Cek jika tombol peserta ditekan
                if (tombolSudahDitekan)
                {
                    break;
                }
            }
        }
        else if (timer >= 15000 && !tombolSudahDitekan)
        {
            standby = false;
            Serial.println("Masuk ke if timer >= 15000 && !tombolSudahDitekan");
        }
    }

    if (tombolSudahDitekan)
    {
        // Serial.println("Masuk ke if tombolSudahDitekan");
        // for (int j = 0; j < 3; j++)
        // {
        //     if (pesertaAktif != j)
        //     {
        //         digitalWrite(common_pins[j], HIGH);
        //     }
        // }
        for (int i = 0; i < 3; i++)
        {
            if (pesertaAktif == i)
            {
                tampilkanHuruf(i, i);
            }
        }
        // Cek tombol naik (juri pin 35)
        if (digitalRead(juri[1]) == LOW && prevJuriState[1] == HIGH)
        {
            tambahNilai();
            // tampilkanAngka(1, pesertaAktif);
            Serial.println("Naik");
            // delay(5000);
            standby = false;
            tombolSudahDitekan = false;
            pesertaAktif = -1;
        }
        // Cek tombol turun (juri pin 32)
        if (digitalRead(juri[2]) == LOW && prevJuriState[2] == HIGH && kondisi != 0)
        {
            kondisi = 0;
            Serial.println("Turun ditekan dan kondisi = " + String(kondisi) + " buttonSudahDitekan = " + String(tombolSudahDitekan));
            kurangNilai();
            jumlahPesertaAktif--;
            pesertaStates.setInactive(pesertaAktif);
            Serial.println("Jumlah peserta aktif: " + String(jumlahPesertaAktif));
            Serial.println("Peserta ke " + String(pesertaAktif) + " Nilainya berkurang menjadi " + String(nilai_peserta[pesertaAktif]));
            Serial.println("Peserta ke " + String(pesertaAktif) + " statusnya menjadi " + String(pesertaStates.isActive(pesertaAktif)));
            startTime = millis();
        }

        if (kondisi == 0 && tombolSudahDitekan)
        {
            if (jumlahPesertaAktif > 0)
            {
                timer = millis() - startTime;
                Serial.println("Looping 3 selama " + String(timer) + " detik");

                for (int i = 0; i < 3; i++)
                {
                    if (pesertaAktif != i)
                    {
                        tampilkanHuruf(i, i);
                        delay(5);
                    }
                    else
                    {
                        tampilkanAngka(nilai_peserta[i], i);
                        // delay(5000);
                    }
                    

                    if (timer <= 3000 && tombolSudahDitekan)
                    {
                    }
                    else
                    {
                        kondisi = -1;
                        standby = false;
                        tombolSudahDitekan = false;
                        pesertaAktif = -1;
                    }

                    if (false)
                    {
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
                }
                delay(5);
            }
        }

        // Cek apakah tombol standby ditekan
        if (digitalRead(juri[0]) == LOW && prevJuriState[0] == HIGH)
        {
            standby = true;
            Serial.println("Tombol standby ditekan");
        }
        for (int i = 0; i < 3; i++)
        {
            prevJuriState[i] = digitalRead(juri[i]);
        }
        // Delay kecil untuk debouncing
        delay(50);
    }
    