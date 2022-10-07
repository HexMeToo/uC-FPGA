library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Przerzutnik is
	port(	clk: in std_logic;
			D:	in std_logic;
			Q: out std_logic_vector(15 downto 0));
end Przerzutnik;

architecture Behavioral of Przerzutnik is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			Q(0) <= D;
			Q(1) <= D;
			Q(2) <= D;
			Q(3) <= D;
			Q(4) <= D;
			Q(5) <= D;
			Q(6) <= D;
			Q(7) <= D;
			Q(8) <= D;
			Q(9) <= D;
			Q(10) <= D;
			Q(11) <= D;
			Q(12) <= D;
		 	Q(13) <= D;
			Q(14) <= D;
			Q(15) <= D;
		end if;
	end process;
	
end Behavioral;