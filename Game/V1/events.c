/* events.c - Isi database event. Tambah event baru cukup tambah satu blok. */
#include <stdlib.h>
#include "events.h"

/* Urutan field Choice: text, result, health, mental, wealth, reputation */
const Event EVENT_DB[] = {
 { 6, 12, "Perundungan di Sekolah",
   "Seorang anak nakal merebut uang jajanmu di depan kelas.",
   {"Melawan balik", "Kamu menang, tapi dipanggil guru BK.", -10, 5, 0, -5},
   {"Melapor ke guru", "Guru menolongmu, tapi kamu dicap pengadu.", 0, -5, 0, 5} },

 { 6, 12, "Lomba Menggambar",
   "Sekolah mengadakan lomba gambar. Hadiahnya lumayan, latihannya melelahkan.",
   {"Ikut dan berlatih keras", "Kamu juara 2! Bangga, walau capek.", -5, 10, 10, 10},
   {"Main game saja", "Santai, tapi kesempatan lewat begitu saja.", 5, 5, 0, -5} },

 { 10, 16, "Godaan Contekan",
   "Ujian besok sulit. Temanmu menawarkan kunci jawaban.",
   {"Terima kunci jawaban", "Nilaimu bagus, tapi hati tidak tenang.", 0, -10, 0, -10},
   {"Belajar semalaman", "Capek, tapi nilaimu jujur dan membanggakan.", -10, 5, 0, 10} },

 { 13, 18, "Ajakan Nongkrong",
   "Geng populer mengajakmu begadang, padahal ada ujian penting.",
   {"Ikut nongkrong", "Seru dan dikenal banyak orang, nilai turun.", -10, 10, -5, 10},
   {"Tetap belajar", "Kamu aman secara akademis, sedikit kesepian.", 0, -5, 5, -5} },

 { 16, 22, "Persimpangan Karier Awal",
   "Ada tawaran beasiswa kuliah, tapi keluargamu butuh penghasilan tambahan.",
   {"Ambil beasiswa", "Masa depan cerah, kantong tipis saat ini.", -5, 5, -10, 15},
   {"Langsung bekerja", "Uang mengalir, tapi pintu pendidikan menyempit.", -5, -5, 20, -5} },

 { 18, 30, "Ajakan Startup",
   "Temanmu mengajak mendirikan startup. Gaji tetap ditinggalkan.",
   {"Ikut startup", "Berisiko! Kamu stres, tapi mulai dikenal.", -10, -10, -15, 20},
   {"Tetap kerja kantoran", "Stabil, tenang, tapi hidup terasa monoton.", 5, 5, 10, -5} },

 { 22, 35, "Lembur Tanpa Henti",
   "Atasan memberi proyek besar. Bonus menggiurkan, tidur jadi korban.",
   {"Ambil proyek", "Bonus cair, badanmu mulai protes.", -20, -15, 25, 10},
   {"Tolak halus", "Istirahatmu cukup, atasan agak kecewa.", 10, 10, -5, -10} },

 { 26, 40, "Pasangan Hidup",
   "Pasanganmu mengajak menikah, sementara kariermu sedang menanjak.",
   {"Menikah sekarang", "Hidup hangat, pengeluaran bertambah.", 5, 15, -15, 10},
   {"Tunda demi karier", "Kariermu melesat, hatimu agak sepi.", 0, -10, 15, 5} },

 { 28, 45, "Teman Pinjam Uang",
   "Sahabat lama meminta pinjaman uang dalam jumlah besar.",
   {"Pinjamkan", "Persahabatan erat, tapi uangmu entah kembali kapan.", 0, 5, -20, 15},
   {"Tolak", "Dompetmu aman, persahabatan merenggang.", 0, -10, 0, -10} },

 { 35, 55, "Investasi Kripto",
   "Rekan kerja memamerkan keuntungan kripto yang fantastis.",
   {"Investasi besar", "Harga anjlok! Tapi kamu belajar banyak.", -5, -15, -30, -5},
   {"Reksa dana aman", "Pelan tapi pasti, hartamu tumbuh.", 0, 5, 15, 0} },

 { 40, 65, "Peringatan Dokter",
   "Hasil cek kesehatan menunjukkan tekanan darah tinggi.",
   {"Ubah gaya hidup", "Berat di awal, tapi tubuhmu pulih.", 20, 5, -5, 0},
   {"Abaikan, sibuk kerja", "Uang terus mengalir, tubuhmu merana.", -25, -5, 10, 0} },

 { 50, 70, "Masa Pensiun",
   "Kamu mendekati pensiun. Apa yang ingin kamu lakukan dengan waktu luang?",
   {"Mentoring anak muda", "Kamu dihormati sebagai sesepuh.", 0, 10, -5, 20},
   {"Keliling dunia", "Kenangan indah, tabunganmu menipis.", 10, 15, -20, 0} },

 { 60, 70, "Warisan Hidup",
   "Keluargamu menanyakan rencana warisan dan wasiatmu.",
   {"Bagi rata & damai", "Keluarga rukun dan penuh syukur.", 0, 15, -10, 10},
   {"Simpan untuk diri sendiri", "Kamu aman, hubungan keluarga dingin.", 0, -10, 10, -15} },

 { 6, 70, "Dompet di Jalan",
   "Kamu menemukan dompet berisi uang dan KTP pemiliknya.",
   {"Kembalikan", "Pemiliknya berterima kasih tulus.", 0, 10, -5, 15},
   {"Ambil uangnya", "Dompet tebal, nurani menggigit.", 0, -15, 15, -10} }
};

const int EVENT_COUNT = sizeof(EVENT_DB) / sizeof(EVENT_DB[0]);

/* Cari event yang cocok usia & belum dipakai; jika habis, boleh pakai ulang */
int events_pick(int age, int used[]) {
    int cand[64], n = 0, i;
    for (i = 0; i < EVENT_COUNT; i++)          /* pass 1: hanya yang belum dipakai */
        if (!used[i] && age >= EVENT_DB[i].min_age && age <= EVENT_DB[i].max_age)
            cand[n++] = i;
    if (n == 0)                                /* pass 2: izinkan pengulangan */
        for (i = 0; i < EVENT_COUNT; i++)
            if (age >= EVENT_DB[i].min_age && age <= EVENT_DB[i].max_age)
                cand[n++] = i;
    return cand[rand() % n];
}
