
enum TYPE OF CAR = {
     
" Кабриолет: Москвич-400-420А.
 Купе (в Италии-берлинетта): Порше-944-турбо, Шкода-рапид-130.
 Купе-кабриолет (встреч. редко): Порше-914, Пежо-304.
 Купе-хардтоп (иногда хардтоп,купе): Рено-17ТС,Шкода-1000МБХ,
      Мерседес-Бенц-380СЕК.
 Тарга (разновидность купе): Феррари-308ГТС.
 Ландо (иногда ландоле, встреч. редко): Руссо-Балт-К12-20.
 Лимузин: ЗИЛ-4104, ЗИС-101.
 Пикап: Тарпан-233, ИЖ-27151.
 Пульман-лимузин (встреч. редко): Мерседес-Бенц-600-пульман.
 Родстер (иногда спайдер): Морган-плюс-8, Альфа-Ромео-спайдер.
 Седан (в США и Японии-нотчбек, в Англии-салун, в Италии-берлина):
      Мазерати-битурбо, ФИАТ-оджи, Фольксваген-джетта.
 Универсал (в США-стэйшн вэгон, в Италии-фамильяле, в Англии-эстейт,
      во Франции-бреак): Фольксваген-пассат, ВАЗ-2104, ВАЗ-2121.
 Фаэтон (устаревшее торпедо): Фольксваген-гольф-ГЛС,
      Паккард-твин-сикс, Майбах-СВ42, ЗИС-102, ГАЗ-69А.
 Хэтчбек (также комби, лифтбек, свингбек): Фольксваген-пассат,
      ВАЗ-2108, СЕАТ-ивиса, Москвич-2141."
      
  limousine, sedan, cupe, landau, cabriolet, cupe cabriolet, phaeton,
  rodster, poolman limousine, sedan hardtop, cupe hardtop, cupe targa,
  embulens, universal, pickup, hatchback
  };

enum NUMBER OF DOORS = {
	4, 2, odd
  };

enum BASE = {

" Расстояние между передней и задней осями автомобиля"

	long, normal, short
  };

enum BODY = {
	closed, partially opened, completely opened, nonstandart,
  cargo passenger
  };

enum PARTITION = {
" Перегородка между первым и последующими рядами седений"
	yes, no
  };

enum NUMBER OF ROWS = {
	1, 2, 3
  };

enum ATTACHED SEATS = {
	yes, no
  };

enum ROOF = {
	oblique, no
  };


/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 1 _____*/
BODY = closed            -> TYPE OF CAR = limousine |
                              TYPE OF CAR = sedan   |
                              TYPE OF CAR = cupe    ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 2 _____*/
BODY = partially opened  -> TYPE OF CAR = landau         |
                              TYPE OF CAR = cabriolet     |
                              TYPE OF CAR = cupe cabriolet;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 3 _____*/
BODY = completely opened -> TYPE OF CAR = phaeton |
                              TYPE OF CAR = rodster ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 4 _____*/
BODY = nonstandart      -> TYPE OF CAR = poolman limousine |
                              TYPE OF CAR = sedan hardtop |
                              TYPE OF CAR = cupe hardtop |
                              TYPE OF CAR = cupe targa ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 5 _____*/
BODY = cargo passenger -> TYPE OF CAR = embulens |
                              TYPE OF CAR = universal |
                              TYPE OF CAR = pickup |
                              TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 6 _____*/
BASE = long  ->  TYPE OF CAR = limousine |
                       TYPE OF CAR = landau |
                       TYPE OF CAR = phaeton |
                       TYPE OF CAR = poolman limousine |
                       TYPE OF CAR = embulens ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 7 _____*/
BASE = normal  ->  TYPE OF CAR = sedan |
                       TYPE OF CAR = cabriolet |
                       TYPE OF CAR = phaeton |
                       TYPE OF CAR = sedan hardtop |
                       TYPE OF CAR = cupe hardtop |
                       TYPE OF CAR = universal |
                       TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 8 _____*/
BASE = short ->  TYPE OF CAR = cupe |
                       TYPE OF CAR = cupe cabriolet |
                       TYPE OF CAR = rodster |
                       TYPE OF CAR = cupe targa |
                       TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 9 _____*/
NUMBER OF DOORS = 4        -> TYPE OF CAR = limousine |
                                TYPE OF CAR = sedan |
                                TYPE OF CAR = landau |
                                TYPE OF CAR = cabriolet |
                                TYPE OF CAR = phaeton |
                                TYPE OF CAR = poolman limousine |
                                TYPE OF CAR = sedan hardtop |
                                TYPE OF CAR = embulens |
                                TYPE OF CAR = universal |
                                TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 10_____*/
NUMBER OF DOORS = 2        -> TYPE OF CAR = sedan |
                                TYPE OF CAR = cupe |
                                TYPE OF CAR = cabriolet |
                                TYPE OF CAR = cupe cabriolet |
                                TYPE OF CAR = phaeton |
                                TYPE OF CAR = rodster |
                                TYPE OF CAR = cupe hardtop |
                                TYPE OF CAR = cupe targa |
                                TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 11_____*/
NUMBER OF DOORS = odd -> TYPE OF CAR = hatchback |
                                TYPE OF CAR = embulens |
                                TYPE OF CAR = universal ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 12_____*/
PARTITION = yes -> TYPE OF CAR = limousine |
                              TYPE OF CAR = landau |
                              TYPE OF CAR = phaeton |
                              TYPE OF CAR = poolman limousine |
                              TYPE OF CAR = embulens |
                              TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 13_____*/
PARTITION = no  -> TYPE OF CAR = sedan |
                              TYPE OF CAR = cupe |
                              TYPE OF CAR = cabriolet |
                              TYPE OF CAR = cupe cabriolet |
                              TYPE OF CAR = phaeton |
                              TYPE OF CAR = rodster |
                              TYPE OF CAR = sedan hardtop |
                              TYPE OF CAR = cupe hardtop |
                              TYPE OF CAR = cupe targa |
                              TYPE OF CAR = universal |
                              TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 14_____*/
NUMBER OF ROWS = 1 -> TYPE OF CAR = embulens |
                        TYPE OF CAR = pickup ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 15_____*/
NUMBER OF ROWS = 3 -> TYPE OF CAR = poolman limousine ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 16_____*/
ATTACHED SEATS = yes -> TYPE OF CAR = limousine |
                             TYPE OF CAR = landau |
                             TYPE OF CAR = phaeton ;
/*--------------------------------------------------------------------------*/
                                                      /*_____ПРАВИЛО 17_____*/
ROOF = oblique -> TYPE OF CAR = hatchback ;
/*--------------------------------------------------------------------------*/