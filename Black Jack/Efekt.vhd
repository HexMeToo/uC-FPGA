library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Efekt is
	port(	CLK: in std_logic;
			A: in std_logic_vector(2 downto 0);
			W1: in std_logic_vector(3 downto 0);
			W2: in std_logic_vector(3 downto 0);
			Q1: buffer std_logic_vector(3 downto 0);
			Q2: buffer std_logic_vector(3 downto 0));
end Efekt;

architecture Behavioral of Efekt is
begin
	process(CLK, A)
	begin
		case A is
		when "011" =>
			if CLK = '1' then
				Q1 <= W1;
				Q2 <= W2;
			else
				Q1 <= "1100";
				Q2 <= "1100";
			end if;
		when others =>
				Q1 <= W1;
				Q2 <= W2;
		end case;
	end process;
end Behavioral;