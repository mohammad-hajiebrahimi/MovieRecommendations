# سیستم پیشنهاد دهنده فیلم (ChiBebinam)

## ساختار فایل‌ها

### فایل فیلم‌ها (films.csv)
```
Name,Director,Cast,Genre,IMDb
La La Land,Damien Chazelle,Ryan Gosling,Drama,8
Interstellar,Christopher Nolan,Matthew McConaughey,Science Fiction,9
...
```

### فایل کاربران (users.csv)
```
Username,Watched,Rating
Ali,Inception;Interstellar;La La Land,Perfect;Average;Poor
Fatima,Superbad;The Mask;Liar Liar,Perfect;Average;Perfect
...
```

## نحوه کامپایل و اجرا

### کامپایل برنامه
```bash
make
```

### اجرای برنامه
```bash
./ChiBebinam users.csv films.csv
```

سپس در برنامه:
```
دستور: genre_recommendation "Ali" "Drama"
```
