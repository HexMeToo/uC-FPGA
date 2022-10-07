library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Oczko is
	port(	clk: in std_logic;
			Led: out std_logic);
			--Q: buffer std_logic_vector (15 downto 0);
			--Q_W: buffer std_logic_vector (15 downto 0));
end Oczko;

architecture Behavioral of Oczko is
--signal adres : std_logic_vector(3 downto 0);
--signal CLK_P : std_logic;
begin

--	IC1 : entity work.Licznik_Kolumny port map(
--   clk => CLK_P);
--	
--	IC2 : entity work.Dekoder_Kolumny port map(
--   clk => CLK_P,
--	A => adres,
--	Q => Q
--	);
	
	IC3 : entity work.Preskaler port map(
   clk => clk,
	Q => Led
	);
	
--	process(clk)
--	begin
--		if(clk'event and clk = '1') then
--		Q_W <= "1111111111111111";
--		end if;
--	end process;
	
end Behavioral;