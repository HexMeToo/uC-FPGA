library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Debouncer is
	port(	CLK: in std_logic;
			I: in std_logic;
			O: buffer std_logic);
end Debouncer;

architecture Behavioral of Debouncer is
signal R: std_logic := '1';
signal Delay: std_logic := '1';
begin

	IC0: entity work.RS port map
	(
		CLK => CLK,
		S => I,
		R => R,
		Q => O
	);
	
	IC1: entity work.Licznik_Delay port map
	(
		CLK => CLK,
		Start => I,
		Q => Delay
	);

	process(I, Delay)
	begin
		R <= Delay nand I;
	end process;
end Behavioral;