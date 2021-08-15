SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       '             Equal (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Equal( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Equal,

       '          NotEqual (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           NotEqual ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS NotEqual,

       '----------------------------------------------------------' AS Spacer_2
  FROM Test_DT_Period T1, Test_DT_Period T2
 WHERE T1.Id IN ('A', 'D' )
   AND T2.Id IN ('A', 'D' );
