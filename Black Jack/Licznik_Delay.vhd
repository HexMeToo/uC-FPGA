library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Licznik_Delay is
	port(	clk: in std_logic;
			Start: in std_logic;
			Q: out std_logic);
end Licznik_Delay;

architecture Behavioral of Licznik_Delay is
signal Licznik: std_logic_vector (17 downto 0) := "000000000000000000";
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			if Licznik  = 0 then
				Licznik <= Licznik;
				Q <= '1';
			else
				Licznik <= Licznik + 1;
				Q <= '0';
			end if;
			
			if Start = '0' and Licznik = 0 then
				Licznik <= Licznik + 1;
				Q <= '0';
			end if;
		end if;
	end process;
end Behavioral;