library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Dekoder_Kolumny is
	port(	clk: in std_logic;
			A: in std_logic_vector(3 downto 0);
			Q: buffer std_logic_vector(15 downto 0) );
end Dekoder_Kolumny;

architecture Behavioral of Dekoder_Kolumny is
begin
	process(A)
	begin
		--if clk'event and clk = '1' then
			case A is
				when "0000" =>   Q <= "1111111111111110";
				when "0001" =>   Q <= "1111111111111101";
				when "0010" =>   Q <= "1111111111111011";
				when "0011" =>   Q <= "1111111111110111";
				when "0100" =>   Q <= "1111111111101111";
				when "0101" =>   Q <= "1111111111011111";
				when "0110" =>   Q <= "1111111110111111";
				when "0111" =>   Q <= "1111111101111111";
				when "1000" =>   Q <= "1111111011111111";
				when "1001" =>   Q <= "1111110111111111";
				when "1010" =>   Q <= "1111101111111111";
				when "1011" =>   Q <= "1111011111111111";
				when "1100" =>   Q <= "1110111111111111";
				when "1101" =>   Q <= "1101111111111111";
				when "1110" =>   Q <= "1011111111111111";
				when "1111" =>   Q <= "0111111111111111";
				when others =>   Q <= "1111111111111110";
			end case;
		--end if;
	end process;
end Behavioral;