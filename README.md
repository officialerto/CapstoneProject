**DONDURULMUŞ ÜRÜN TAKİP SİSTEMİ**

Projenin ilk adımı olarak, donanımların breadboard üzerine yerleştirilmesi ve gerekli bağlantıların yapılması gerekmektedir. NodeMCU, AMG8833 ve MPU6050 sensörleri, breadboard üzerinde belirlenen konumlara yerleştirilir. 

AMG8833 Kızılötesi Sıcaklık Sensörü ve MPU6050 İvmeölçer ve Jiroskop Sensörü, NodeMCU kartına bağlanmak üzere uygun pinleri lehimlenir. Bu lehimleme işlemi sırasında, her bir pinin doğru şekilde bağlantı noktalarına yerleştirildiğinden ve sağlam bir bağlantı sağlandığından emin olunmalıdır. Bağlantıların doğruluğunu test etmek için bir multimetre kullanılabilir. Lehimleme işlemi tamamlandıktan sonra, breadboard üzerindeki bağlantılar kontrol edilir ve güç kaynağı ile donanımın çalışıp çalışmadığı test edilir. Bu aşamada, sensörlerin doğru veri sağladığından emin olmak için basit bir test kodu kullanılabilir.

Projede, donanımlardan toplanan verilerin işlenmesi ve kullanıcıya sunulması için çeşitli yazılım teknolojileri ve kütüphaneler kullanılmıştır. Bu süreçte WebSocket, HTML, CSS, JavaScript ve HTTP protokolü gibi temel web teknolojilerinden faydalanılmıştır.

Donanımlar tarafından toplanan verilerin gerçek zamanlı olarak işlenmesi ve sunucuya aktarılması için WebSocket teknolojisi tercih edilmiştir. WebSocket, çift yönlü iletişimi sağlayarak verilerin sürekli ve anlık olarak aktarılmasına imkan tanır. NodeMCU üzerindeki veriler, WebSocket aracılığıyla sunucuya gönderilir ve bu sayede veri aktarımında yüksek performans ve düşük gecikme sağlanır.

Sunucu tarafında, HTTP protokolü kullanılarak istemciden gelen talepler işlenir. HTTP protokolü, istemci-sunucu arasındaki iletişimi düzenler ve verilerin güvenli bir şekilde aktarılmasını sağlar. Veriler sunucuya ulaştıktan sonra, bu veriler işlenir ve depolanır.

HTML, CSS ve JavaScript ile oluşturulan kullanıcı arayüzü, bu verileri görsel olarak sunmaktadır. Verilerin görselleştirilmesinde sıcaklıklar, belirli aralıklara göre mavi, yeşil ve kırmızı olmak üzere üç renge ayrılmıştır. Bu renk kodlaması, kullanıcıların sıcaklık değişimlerini kolayca takip etmelerini sağlamaktadır.

Projede kullanılan kütüphaneler arasında, WebSocket iletişimi için "WebSocketsServer" kütüphanesi, HTTP isteklerini işlemek için "express" gibi popüler kütüphaneler yer almaktadır. Bu kütüphaneler, projenin verimli ve sürdürülebilir bir şekilde geliştirilmesine katkı sağlar.

Ayrıca, MPU6050'den alınan açısal veriler Pisagor teoremi kullanılarak işlenmekte ve bu açı değerlerinin 1.35'ten büyük olması durumunda "Bir Şey Düştü!" uyarısı  verilmektedir. 

Bu formül, üç boyutlu uzayda bir noktadan başka bir noktaya olan düz çizgi mesafesini hesaplamak için Pisagor teoreminden türetilmiştir. İvme vektörünün büyüklüğünü bulmak için, her bir bileşenin karesini alıp toplayarak ve ardından karekökünü alarak net ivmeyi hesaplamış olursunuz.

Bu özellik, dondurulmuş ürünlerin taşınması veya depolanması sırasında olası düşme veya darbe durumlarının hızlıca tespit edilmesine olanak tanır.
Proje kapsamında, toplanan verilerin işlenmesi ve saklanması için Node.js 5000 portunda bir sunucu oluşturulmuştur. Node.js, JavaScript tabanlı bir çalışma zamanı ortamıdır ve hızlı, hafif ve etkili bir şekilde sunucu tarafı uygulamaları geliştirmeye olanak tanır. Bu sunucu, gelen verileri işleyerek gerektiğinde kullanıcı arayüzüne iletmekte ve aynı zamanda MySQL veritabanına kaydetmektedir.

Verilerin MySQL veritabanına aktarılması için XAMPP gibi bir web sunucusu yazılımı kullanılmıştır. XAMPP, Apache sunucusu, MySQL veritabanı ve PHP programlama dilini bir araya getiren bir yazılım paketidir. Bu paket, sunucu kurulumu ve yapılandırması için kolay bir arayüz sunar ve geliştirme ortamını hızla oluşturur.

Node.js sunucusu, gelen verileri işleyerek uygun MySQL sorgularını oluşturur ve bu sorguları XAMPP üzerinde çalışan MySQL veritabanına ileterek verilerin kaydedilmesini sağlar. Bu sayede, kullanıcıların topladığı veriler uzun süreli depolanabilir ve daha sonra analiz veya raporlama amaçlarıyla kullanılabilir. Ardından, başarılı bir şekilde çalıştığında, Arduino IDE üzerinde "Data Inserted" bilgisini görüntüler, böylece verilerin başarıyla kaydedildiğini ve veritabanına eklendiğini kullanıcıya bildirir.

Bu geri bildirim, sistemin doğru bir şekilde çalıştığını ve veri aktarımının sorunsuz bir şekilde gerçekleştiğini doğrulamak için önemlidir.

Tüm bu süreç, projenin veri toplama, işleme, depolama ve sunma aşamalarını entegre ederek kullanıcıya kapsamlı bir deneyim sunar.
