const express = require('express'); // Express framework'ünü dahil et
const mysql = require('mysql'); // MySQL veritabanı kütüphanesini dahil et
const bodyParser = require('body-parser'); // Body-parser kütüphanesini dahil et

const app = express(); // Express uygulamasını başlat
app.use(bodyParser.json()); // JSON formatındaki gövdeyi ayrıştırma işlemini etkinleştir

// MySQL bağlantı ayarları
const db = mysql.createConnection({
  host: 'localhost', // Veritabanı sunucusunun adresi
  user: 'root', // Veritabanı kullanıcı adı
  password: '123', // Veritabanı şifresi
  database: 'sensor_data' // Kullanılacak veritabanı adı
});

// Veritabanına bağlan
db.connect(err => {
  if (err) throw err; // Hata varsa, hatayı göster
  console.log('MySQL Connected...'); // Bağlantı başarılıysa, mesaj göster
});

// POST isteği için /data rotasını tanımla
app.post('/data', (req, res) => {
  let data = req.body; // İstek gövdesindeki veriyi al

  // Veritabanına veri eklemek için SQL sorgusu
  let sql = 'INSERT INTO sensor_data (acc_x, acc_y, acc_z, temperatures, alert) VALUES (?, ?, ?, ?, ?)';
  let values = [
    data.acceleration.x, // İvme x değeri
    data.acceleration.y, // İvme y değeri
    data.acceleration.z, // İvme z değeri
    JSON.stringify(data.temperature), // Sıcaklık değerlerini JSON formatında sakla
    data.alert || '-' // 'alert' değeri yoksa 'Her şey yolunda' ekle
  ];
  
  // Veritabanına sorguyu gönder
  db.query(sql, values, (err, result) => {
    if (err) throw err; // Hata varsa, hatayı göster
    res.send('Data inserted'); // Başarılıysa, mesaj göster
  });
});

const PORT = process.env.PORT || 5000; // Sunucu portunu belirle
app.listen(PORT, () => console.log(`Server started on port ${PORT}`)); // Sunucuyu başlat ve mesaj göster
