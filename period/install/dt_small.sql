
SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1
, '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
        CompareString( T1.When, T2.When )::LVARCHAR AS CompareString

, '             Equal (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
	Equal( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Equal

-- , 'Length( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
-- Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union

,        'Period_Interval( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
	Period_Interval(GetIntersect (T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Intersect_Interval

  FROM Test_DT_Period T1, Test_DT_Period T2;
