# سیستم پیشنهاد دهنده فیلم (ChiBebinam)

این پروژه یک موتور پیشنهاد دهنده فیلم است که با استفاده از اطلاعات کاربران و فیلم‌ها، پیشنهادات شخصی‌سازی شده ارائه می‌دهد.

## ویژگی‌ها

- خواندن داده‌ها از فایل‌های CSV
- الگوریتم پیشنهاد بر اساس ژانر
- پشتیبانی از کاربران موجود و جدید
- محاسبه امتیاز بر اساس سابقه کاربر
- مرتب‌سازی هوشمند پیشنهادات

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

یا با استفاده از Makefile:
```bash
make run
```

## دستورات برنامه

پس از اجرای برنامه، می‌توانید از دستورات زیر استفاده کنید:

- `users` - نمایش لیست کاربران
- `films` - نمایش لیست فیلم‌ها
- `genre_recommendation "<Username>" "<Genre>"` - پیشنهاد بر اساس ژانر
- `exit` - خروج از برنامه

## مثال استفاده

```bash
./ChiBebinam users.csv films.csv
```

سپس در برنامه:
```
دستور: genre_recommendation "Ali" "Drama"
```

## الگوریتم پیشنهاد

### برای کاربران موجود:
برنامه از فرمول زیر برای محاسبه امتیاز استفاده می‌کند:

```
Score = (IMDb * 0.7) + (DirectorEffect * 0.5) + (CastEffect * 0.6)
```

**DirectorEffect:**
- Perfect: 10
- Average: 5  
- Poor: -1

**CastEffect:**
- Perfect: 7
- Average: 4
- Poor: -2

### برای کاربران جدید:
فقط بر اساس امتیاز IMDb مرتب‌سازی می‌شود.

## ژانرهای مجاز

- Action
- Comedy
- Drama
- Horror
- Science Fiction

## مرتب‌سازی

1. بر اساس امتیاز محاسبه شده (نزولی)
2. در صورت یکسان بودن امتیاز، بر اساس IMDb (نزولی)
3. در صورت یکسان بودن IMDb، بر اساس نام فیلم (صعودی)

## پاک کردن فایل‌های کامپایل شده

```bash
make clean
``` 