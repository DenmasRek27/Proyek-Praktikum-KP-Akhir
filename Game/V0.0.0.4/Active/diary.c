/* Antarmuka buku diary (Win32 GDI, tanpa library tambahan).
 * Build: gcc diary.c player.c ending.c util.c -o diary.exe -lgdi32 -mwindows
 * Pakai ulang Player/ubahStat/cekGagal/cekMenang dari proyek lama. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "ending.h"

#define W 900
#define H 640
#define CW 220
#define CH 130
#define CARD_Y 450
#define PAGE_MAX 380          /* lewat ini, halaman dibalik */
#define N_AUTO (-1)           /* next = maju ke hari berikutnya */
#define NO_STAT (-1)

enum { N_PAGI, N_HUTAN, N_BURU, N_SERIGALA, N_KAYU, N_HERBAL, N_DESA, N_LUANG };
enum { PH_COVER, PH_OPEN, PH_TYPE, PH_CARDS, PH_PICK, PH_END };

typedef struct { const char *label; int stat, delta, next; } Choice;
typedef struct { const char *art, *text; int n; Choice c[3]; } Node;

/* Pohon aksi: tambah node di sini. Label = lanjutan kalimat orang pertama. */
static const Node NODES[] = {
 /* PAGI */ { "    \\ | /\n  -- ( ) --\n    / | \\\n\n ~~~~~~~~~~~", "Pagi ini aku akan...", 3, {
   { "pergi ke hutan.", NO_STAT, 0, N_HUTAN },
   { "turun ke desa.", NO_STAT, 0, N_DESA },
   { "bermain dan mengamati sekitar.", STAT_MENTAL, 3, N_AUTO } } },
 /* HUTAN */ { "   ^    ^^\n  /|\\  /||\\\n /_|_\\/_||_\\\n   |    ||", "Pepohonan di sini rapat dan sunyi. Aku akan...", 3, {
   { "berburu.", NO_STAT, 0, N_BURU },
   { "menebang pohon.", NO_STAT, 0, N_KAYU },
   { "mencari herbal.", NO_STAT, 0, N_HERBAL } } },
 /* BURU */ { "  /\\_/\\\n ( o.o )\n  > ^ <", "Ada jejak segar di tanah lembap. Aku akan...", 3, {
   { "mengikutinya pelan-pelan.", STAT_KEKAYAAN, 6, N_AUTO },
   { "memasang jerat lalu menunggu.", STAT_KEKAYAAN, 3, N_AUTO },
   { "menoleh, ada geraman di semak.", NO_STAT, 0, N_SERIGALA } } },
 /* SERIGALA (contoh combat, masih placeholder) */ { "  |\\___/|\n  ( o o )\n  /  v  \\", "Seekor serigala lapar muncul dari semak. Aku akan...", 2, {
   { "melawannya dengan tongkat.", STAT_KESEHATAN, -6, N_AUTO },
   { "mundur perlahan sambil menatapnya.", STAT_MENTAL, -2, N_AUTO } } },
 /* KAYU */ { "    ||\n   /||\\\n  / || \\", "Di tepi hutan banyak kayu. Aku akan...", 2, {
   { "mengumpulkan ranting kering.", STAT_KEKAYAAN, 2, N_AUTO },
   { "menebang pohon muda.", STAT_KEKAYAAN, 5, N_AUTO } } },
 /* HERBAL */ { "   \\|/\n  --*--\n   /|\\", "Aroma daun liar tercium. Aku akan...", 2, {
   { "memetik secukupnya.", STAT_KESEHATAN, 5, N_AUTO },
   { "membawa pulang sebanyak mungkin.", STAT_KEKAYAAN, 4, N_AUTO } } },
 /* DESA */ { "   /\\    /\\\n  /__\\  /__\\\n  |[]|  |[]|", "Desa ramai pagi ini. Aku akan...", 3, {
   { "menyapa para warga.", STAT_REPUTASI, 4, N_AUTO },
   { "membantu di ladang.", STAT_KEKAYAAN, 4, N_AUTO },
   { "hanya lewat sambil mengamati.", STAT_MENTAL, 2, N_AUTO } } },
 /* LUANG (hari acak: dilema kartu) */ { " ~ ~ ~ ~ ~ ~\n  ~ ~ ~ ~ ~ ~\n ~ ~ ~ ~ ~ ~", "Di waktu luangku, aku sedang bersantai di pesisir sungai.", 2, {
   { "Aku menghabiskan waktu di sini.", STAT_MENTAL, 5, N_AUTO },
   { "Aku memilih untuk kembali ke desa.", STAT_REPUTASI, 3, N_AUTO } } },
};

static const char *END_TXT[] = { "",
  "Tubuhku kolaps oleh luka dan penyakit. Aku tak bangun lagi.",
  "Pikiranku runtuh. Orang desa melihatku mengigau di hutan.",
  "Aku jatuh miskin dan kelaparan tanpa ada yang menolong.",
  "Hidup Mandiri Seimbang. Aku bertahan sendiri, tubuh, pikiran, dan harta terjaga.",
  "Aku menemukan tempat di tengah komunitas. Menjadi rakyat biasa.",
  "Aku mencapai umur 106. Hidup panjang, meski tak menonjol." };

typedef struct { float x, y, a, tx, ty, ta; } Card;

static Player pl;
static int hari = 1, node, phase = PH_COVER, ncd, hover = -1, chosen = -1;
static char page[1400];
static int plen, shown;
static float acc, openT;
static const char *art = "";
static Card cd[3];
static HFONT fText, fArt, fCard;

static COLORREF mix(COLORREF a, COLORREF b, float t) {
    return RGB(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t,
               GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t,
               GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t);
}

static void enterNode(int n) {
    char buf[300];
    int l;
    if (n == N_PAGI)
        snprintf(buf, sizeof buf, "Aku sudah berumur %d, hari ke-%d. Cuaca cerah! %s", pl.umur, hari, NODES[n].text);
    else
        snprintf(buf, sizeof buf, "%s", NODES[n].text);
    l = (int)strlen(buf);
    if (plen + l + 2 > PAGE_MAX) { page[0] = 0; plen = shown = 0; } /* balik halaman */
    if (plen) { strcat(page, "\n"); plen++; }
    strcat(page, buf);
    plen += l;
    node = n; art = NODES[n].art; ncd = 0; phase = PH_TYPE;
}

static int advance(Ending *e) {
    if (++hari > 5) {
        hari = 1;
        ubahStat(&pl, STAT_KEKAYAAN, -3);
        ubahStat(&pl, STAT_KESEHATAN, 10);
        pl.umur += pl.umur < UMUR_DEWASA ? 5 : 10;
        *e = cekGagal(&pl);
        if (*e == ENDING_NONE) *e = cekMenang(&pl);
    }
    return hari <= 3 ? N_PAGI : N_LUANG;
}

static void finishPick(void) {
    const Choice *c = &NODES[node].c[chosen];
    int nx = c->next;
    Ending e;
    if (c->stat != NO_STAT) ubahStat(&pl, c->stat, c->delta);
    strcat(page, " "); strcat(page, c->label);
    plen = (int)strlen(page);
    e = cekGagal(&pl);
    if (e == ENDING_NONE && nx == N_AUTO) nx = advance(&e);
    if (e != ENDING_NONE) {
        strcpy(page, END_TXT[e]); plen = (int)strlen(page); shown = 0;
        art = ""; ncd = 0; phase = PH_END;
        return;
    }
    enterNode(nx);
}

static void spawnCards(void) {
    int i, n = NODES[node].n, total = n * CW + (n - 1) * 30;
    ncd = n; hover = -1; chosen = -1;
    for (i = 0; i < n; i++) {
        cd[i].tx = (float)((W - total) / 2 + i * (CW + 30));
        cd[i].ty = CARD_Y;
        cd[i].x = cd[i].tx; cd[i].y = 720; cd[i].a = 0; cd[i].ta = 1;
    }
    phase = PH_CARDS;
}

static int hit(int mx, int my) {
    int i;
    for (i = 0; i < ncd; i++)
        if (mx >= cd[i].tx && mx <= cd[i].tx + CW && my >= CARD_Y - 25 && my <= CARD_Y + CH) return i;
    return -1;
}

static void onTick(void) {
    int i, done = 1;
    if (phase == PH_OPEN) {
        openT += 0.016f;
        if (openT >= 1) { page[0] = 0; plen = shown = 0; enterNode(N_PAGI); } /* halaman kosong dulu */
    }
    if (phase == PH_TYPE || phase == PH_END) {
        acc += 0.5f;
        while (acc >= 1 && shown < plen) { shown++; acc -= 1; }
        if (shown >= plen) { acc = 0; if (phase == PH_TYPE) spawnCards(); }
    }
    for (i = 0; i < ncd; i++) {
        cd[i].x += (cd[i].tx - cd[i].x) * 0.15f;
        cd[i].y += (cd[i].ty - cd[i].y) * 0.15f;
        cd[i].a += (cd[i].ta - cd[i].a) * 0.15f;
        if (cd[i].a > 0.05f) done = 0;
    }
    if (phase == PH_PICK && done) finishPick();
}

static void pick(int i) {
    int j;
    chosen = i; phase = PH_PICK;
    for (j = 0; j < ncd; j++) {
        cd[j].ta = 0;
        if (j == i) cd[j].ty = 300;                         /* terangkat ke halaman */
        else cd[j].tx = cd[j].tx < W / 2 ? -300.0f : 1200.0f; /* tersisih */
    }
}

static void draw(HDC dc) {
    RECT r = { 0, 0, W, H };
    char buf[160];
    int i, w;
    const COLORREF DESK = RGB(38, 27, 22), PAPER = RGB(240, 226, 190), INK = RGB(50, 35, 25);
    HBRUSH desk = CreateSolidBrush(DESK);
    FillRect(dc, &r, desk); DeleteObject(desk);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, GetStockObject(DC_BRUSH));
    SelectObject(dc, GetStockObject(DC_PEN));

    if (phase == PH_COVER || phase == PH_OPEN) {
        w = phase == PH_COVER ? 520 : (openT < 0.5f ? (int)(520 * (1 - 2 * openT)) : (int)(780 * (2 * openT - 1)));
        SetDCBrushColor(dc, phase == PH_OPEN && openT >= 0.5f ? PAPER : RGB(110, 60, 35));
        SetDCPenColor(dc, RGB(20, 12, 8));
        RoundRect(dc, W / 2 - w / 2, 120, W / 2 + w / 2, 500, 12, 12);
        if (phase == PH_COVER) {
            SelectObject(dc, fText); SetTextColor(dc, RGB(230, 200, 140));
            SetRect(&r, 0, 250, W, 330);
            DrawTextA(dc, "KISAH DI TANAH PERBATASAN", -1, &r, DT_CENTER | DT_SINGLELINE);
            SelectObject(dc, fArt); SetTextColor(dc, RGB(200, 170, 120));
            SetRect(&r, 0, 400, W, 440);
            DrawTextA(dc, "[ klik untuk membuka buku ]", -1, &r, DT_CENTER | DT_SINGLELINE);
        }
        return;
    }

    sprintf(buf, "Umur %d | Kes %d  Men %d  Kek %d  Rep %d | Karma %d", pl.umur,
            pl.stat[STAT_KESEHATAN], pl.stat[STAT_MENTAL], pl.stat[STAT_KEKAYAAN],
            pl.stat[STAT_REPUTASI], pl.karma);
    SelectObject(dc, fArt); SetTextColor(dc, RGB(210, 190, 150));
    TextOutA(dc, 60, 16, buf, (int)strlen(buf));

    SetDCBrushColor(dc, PAPER); SetDCPenColor(dc, RGB(120, 90, 60));
    RoundRect(dc, 60, 50, 840, 410, 10, 10);
    MoveToEx(dc, 580, 65, NULL); LineTo(dc, 580, 395);

    SetTextColor(dc, INK);
    SelectObject(dc, fText);
    SetRect(&r, 90, 75, 565, 395);
    DrawTextA(dc, page, shown, &r, DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, fArt);
    SetRect(&r, 605, 100, 830, 390);
    DrawTextA(dc, art, -1, &r, DT_NOPREFIX);

    for (i = 0; i < ncd; i++) {
        float f = 1 - cd[i].a;
        SetDCBrushColor(dc, mix(i == hover ? RGB(255, 246, 220) : RGB(235, 218, 180), DESK, f));
        SetDCPenColor(dc, mix(RGB(120, 90, 60), DESK, f));
        RoundRect(dc, (int)cd[i].x, (int)cd[i].y, (int)cd[i].x + CW, (int)cd[i].y + CH, 14, 14);
        SelectObject(dc, fCard); SetTextColor(dc, mix(INK, DESK, f));
        SetRect(&r, (int)cd[i].x + 14, (int)cd[i].y + 14, (int)cd[i].x + CW - 14, (int)cd[i].y + CH - 10);
        DrawTextA(dc, NODES[node].c[i].label, -1, &r, DT_WORDBREAK | DT_NOPREFIX);
    }
}

static LRESULT CALLBACK proc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    int mx = (short)LOWORD(lp), my = (short)HIWORD(lp);
    switch (m) {
    case WM_CREATE:
        fText = CreateFontA(-21, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Segoe Print");
        fCard = CreateFontA(-17, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Segoe Print");
        fArt  = CreateFontA(-15, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, FIXED_PITCH, "Consolas");
        SetTimer(h, 1, 16, NULL);
        return 0;
    case WM_TIMER: onTick(); InvalidateRect(h, NULL, FALSE); return 0;
    case WM_MOUSEMOVE:
        if (phase == PH_CARDS) {
            int i, k = hit(mx, my);
            hover = k;
            for (i = 0; i < ncd; i++) cd[i].ty = (float)(CARD_Y - (i == k ? 22 : 0));
        }
        return 0;
    case WM_LBUTTONDOWN:
        if (phase == PH_COVER) { phase = PH_OPEN; openT = 0; }
        else if (phase == PH_TYPE) shown = plen;
        else if (phase == PH_CARDS) { int k = hit(mx, my); if (k >= 0) pick(k); }
        else if (phase == PH_END) { if (shown >= plen) DestroyWindow(h); else shown = plen; }
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(h, &ps);
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bm = CreateCompatibleBitmap(hdc, W, H), old = SelectObject(mem, bm);
        draw(mem);
        BitBlt(hdc, 0, 0, W, H, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old); DeleteObject(bm); DeleteDC(mem);
        EndPaint(h, &ps);
        return 0; }
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(h, m, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int sw) {
    WNDCLASSA wc;
    RECT r = { 0, 0, W, H };
    DWORD st = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    HWND h;
    MSG msg;
    (void)hp; (void)cl;
    srand((unsigned)time(NULL));
    inisialisasiPlayer(&pl);
    pl.umur = 1;
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = proc; wc.hInstance = hi; wc.lpszClassName = "Diary";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
    AdjustWindowRect(&r, st, FALSE);
    h = CreateWindowA("Diary", "Kisah di Tanah Perbatasan", st, CW_USEDEFAULT, CW_USEDEFAULT,
                      r.right - r.left, r.bottom - r.top, NULL, NULL, hi, NULL);
    ShowWindow(h, sw);
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}
